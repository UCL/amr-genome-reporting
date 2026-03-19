/*
  AMR Genome Reporting CLI
  Copyright (C) 2026  University College London.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "parse_json.h"

#include <stdio.h>
#include <string.h>

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

json_status_t validate_required_string(const cJSON *obj)
{
    if (!obj)
        return ERR_MISSING_FIELD;
    if (!cJSON_IsString(obj))
        return ERR_TYPE_MISMATCH;
    if (obj->valuestring == NULL || strcmp(obj->valuestring,"") == 0)
        return ERR_INVALID_VALUE;

    return OK;
}

json_status_t validate_required_obj(const cJSON *json, const char **req_names)
{
    const cJSON *field = NULL;
    json_status_t rc = OK;
    const char *name = NULL;

    if (json == NULL)
        return ERR_INVALID_VALUE;

    if (!cJSON_IsObject(json))
        return ERR_TYPE_MISMATCH;

    for (size_t i = 0; i < ARRAY_LEN(req_names); i++)
    {
        name = req_names[i];
        field = cJSON_GetObjectItemCaseSensitive(json, name);
        rc = validate_required_string(field);
        if (rc != OK) {
            fprintf(stderr, "Error validating required string '%s': %d\n", name, rc);
            return rc;
        }
    }

    return rc;
}

json_status_t validate_required_objarr(const cJSON *json, const char **req_names)
{
    if (!json)
        return ERR_INVALID_VALUE;

    if (!cJSON_IsArray(json))
        return ERR_TYPE_MISMATCH;

    json_status_t rc = OK;
    cJSON *iterator = NULL;
    
    cJSON_ArrayForEach(iterator, json)
    {
        rc = validate_required_obj(iterator, req_names);
        if (rc != OK)
            cJSON_Delete(iterator);
            return rc;
    }

    cJSON_Delete(iterator);
    return rc;
}

/**
 * @brief Validator for the metadata section of the report json document.
 *
 * Expected json structure:
 * @code
 * {
 *   "author": "Forename Surname",
 *   "label": "Report label",
 *   "version": "v1.0",
 *   "timestamp": "2026-02-03 11:19:00"
 * }
 * @endcode
 *
 * Validation rules:
 * - The input must be a json object
 * - All required fields must be present
 * - All required fields must be a non-empty string as a value
 *
 * @param json Pointer to a cJSON object representing the metadata section.
 * @return OK on success.
 * @return ERR_TYPE_MISMATCH if json is not an object or a field has the wrong
 * type.
 * @return ERR_MISSING_FIELD if a required field is missing.
 * @return ERR_INVALID_VALUE if a field value is empty or invalid.
 *
 * @note The function does not take ownership of @p json. The caller is
 * responsible for managing the life of the cJSON tree.
 */
json_status_t validate_metadata(const cJSON *json)
{
    const char *req_names[] = {
        "author",
        "label",
        "version",
        "timestamp",
    };

    json_status_t status = OK;
    status = validate_required_obj(json, req_names);
    return status;
}

/**
 * @brief Validator for the header section of the report json document.
 *
 * Expected json structure is an array of objects defined as:
 * @code
 * [
 *   {
 *     "label": "Genome identifier",
 *     "value": "Case 1"
 *   }
 * ]
 * @endcode
 *
 * Validation rules:
 * - The input must be a json array
 * - The elements in the array must be objects
 * - The required fields within each object must be present
 * - The value of each field in each object must be a non-empty string
 *
 * @param json Pointer to a cJSON object representing the header section.
 * @return OK on success.
 * @return ERR_TYPE_MISMATCH if json is not an object or a field has the wrong
 * type.
 * @return ERR_MISSING_FIELD if a required field is missing.
 * @return ERR_INVALID_VALUE if a field value is empty or invalid.
 *
 * @note The function does not take ownership of @p json. The caller is
 * responsible for managing the life of the cJSON tree.
 */
json_status_t validate_clinical_header(const cJSON *json)
{
    const char *req_names[] = {
        "label",
        "value"
    };
    json_status_t status = OK;
    status = validate_required_objarr(json, req_names);
    return status;
}

static const validation_section_t schema[] = {
    { "metadata", validate_metadata },
    { "header", validate_clinical_header },
};

json_status_t validate_json_structure(const cJSON *json)
{
  for (size_t i = 0; i < ARRAY_LEN(schema); i++) {
    const validation_section_t *spec = &schema[i];
    // We validate each section separately
    const cJSON *section = cJSON_GetObjectItemCaseSensitive(json, spec->name);

    if (!section) {
        fprintf(stderr, "Missing section %s\n", spec->name);
        return ERR_MISSING_FIELD;
    }

    json_status_t rc = spec->validator(section);
    if (rc != OK) {
      fprintf(stderr, "Error validating section '%s': %d\n", spec->name, rc);
      return rc;
    }
  }
  return OK;
}

report_data_t extract_data(const cJSON *json)
{
    report_data_t data;
    report_data_init(&data);

    data.title = "CLINICAL GENOME REPORT";

    cJSON *header = cJSON_GetObjectItemCaseSensitive(json, "header");
    if (!header) {
        fprintf(stderr, "Missing section header\n");
        return data;
    }

    // Caller must have validated the JSON document already
    cJSON *iterator = NULL;
    cJSON *label = NULL;
    cJSON *value = NULL;
    cJSON_ArrayForEach(iterator, header)
    {
        label = cJSON_GetObjectItemCaseSensitive(iterator, "label");
        value = cJSON_GetObjectItemCaseSensitive(iterator, "value");

        report_data_append_header(&data, label->valuestring, value->valuestring);
    }

    cJSON *clinical_microbiology = cJSON_GetObjectItemCaseSensitive(json, "clinical_microbiology");
    if (!clinical_microbiology) {
        fprintf(stderr, "Missing section clinical_microbiology\n");
        return data;
    }
    cJSON *clinical_summary = cJSON_GetObjectItemCaseSensitive(clinical_microbiology, "summary");
    if (!clinical_summary) {
        fprintf(stderr, "Missing section clinical_microbiology.summary\n");
        return data;
    }
    cJSON *font = NULL;
    cJSON *text = NULL;
    cJSON_ArrayForEach(iterator, clinical_summary)
    {
        font = cJSON_GetObjectItemCaseSensitive(iterator, "font");
        text = cJSON_GetObjectItemCaseSensitive(iterator, "text");

        report_data_append_clinical_microbiology_summary(&data, font->valuestring, text->valuestring);
    }

    cJSON *organism = cJSON_GetObjectItemCaseSensitive(clinical_microbiology, "organism");
    data.clinical_microbiology.organism = organism->valuestring;

    cJSON *bacterial_typing = cJSON_GetObjectItemCaseSensitive(clinical_microbiology, "bacterial_typing");
    if (!bacterial_typing) {
        fprintf(stderr, "Missing section clinical_microbiology.bacterial_typing\n");
        return data;
    }
    cJSON *typing_desc = cJSON_GetObjectItemCaseSensitive(bacterial_typing, "description");
    data.clinical_microbiology.bacterial_typing.description = typing_desc->valuestring;
    cJSON *typing_url = cJSON_GetObjectItemCaseSensitive(bacterial_typing, "url");
    data.clinical_microbiology.bacterial_typing.url = typing_url->valuestring;
    cJSON *typing = cJSON_GetObjectItemCaseSensitive(bacterial_typing, "typing");
    data.clinical_microbiology.bacterial_typing.typing = typing->valuestring;
    cJSON *typing_seq = cJSON_GetObjectItemCaseSensitive(bacterial_typing, "sequence_type");
    data.clinical_microbiology.bacterial_typing.sequence_type = typing_seq->valuestring;
    cJSON *typing_profile = cJSON_GetObjectItemCaseSensitive(bacterial_typing, "profile");
    cJSON *gene = NULL;
    cJSON *allele = NULL;
    cJSON_ArrayForEach(iterator, typing_profile)
    {
        gene = cJSON_GetObjectItemCaseSensitive(iterator, "gene");
        allele = cJSON_GetObjectItemCaseSensitive(iterator, "allele");

        report_data_append_bacterial_typing_profile(&data, gene->valuestring, allele->valuestring);
    }
    cJSON *determinants = cJSON_GetObjectItemCaseSensitive(clinical_microbiology, "antibiotic_resistance_determinants");
    if (!determinants) {
        fprintf(stderr, "Missing section antibiotic_resistance_determinants\n");
        return data;
    }
    cJSON *drug = NULL;
    cJSON *acq_gen = NULL;
    cJSON_ArrayForEach(iterator, determinants)
    {
        drug = cJSON_GetObjectItemCaseSensitive(iterator, "drug_class");
        acq_gen = cJSON_GetObjectItemCaseSensitive(iterator, "acquired_genotypes");

        report_data_append_determinants(&data, drug->valuestring, acq_gen->valuestring);
    }

//    cJSON_Delete(header);
    cJSON_Delete(iterator);
//    cJSON_Delete(label);
//    cJSON_Delete(value);

    return data;
}
