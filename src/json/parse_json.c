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

json_status_t validate_required(const cJSON *json, const char **req_names)
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
            fprintf(stderr, "Error validating metadata '%s': %d\n", name, rc);
            return rc;
        }
    }

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
 * - All required fileds must be a non-empty string as a value.
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
    status = validate_required(json, req_names);
    return status;
}

json_status_t validate_clinical_header(const cJSON *json)
{
    const char *req_names[] = {
        "genome_identifier",
        "isolate_lab_identifier",
        "date_isolation",
        "location",
        "source_type",
        "host_tissue",
        "accession_number",
        "genome_report_date",
        "genome_quality"
    };
    json_status_t status = OK;
    status = validate_required(json, req_names);
    return status;
}

static const validation_section_t schema[] = {
    { "metadata", validate_metadata },
    { "header", validate_clinical_header },
};

json_status_t validate_json_structure(const cJSON *json)
{
  return 0;
}
