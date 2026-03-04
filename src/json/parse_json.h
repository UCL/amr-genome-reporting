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
#ifndef PARSE_JSON_H
#define PARSE_JSON_H

#include <cJSON.h>
#include "../main.h"

/**
 * @enum json_status_t
 *
 * @brief Status codes to be returned by json validation functions.
 *
 * All validators must return one of these status codes.
 */
typedef enum {
  /** Validation succesful */
  OK = 0,

  /** Required field is missing */
  ERR_MISSING_FIELD,

  /** Field has an incorrect type */
  ERR_TYPE_MISMATCH,

  /** Field contains an invalid value */
  ERR_INVALID_VALUE,
} json_status_t;

/**
 * @typedef section_validator_t
 *
 * @brief Function pointer type for validation of a json section.
 *
 * A section validator inspects a given json object or value and
 * verifies that it conforms to the expected structure.
 *
 * The validator must not modify the json tree.
 *
 * @param section Pointer to the json value representing the section
 * to be validated. It must be non-NULL.
 * @return json_status_t indicating the result of the validation.
 */
typedef json_status_t (*section_validator_t)(const cJSON *section);

/**
 * @struct validation_section_s
 *
 * @brief Description of a named json section and its validator.
 *
 * This structure allows for a schema-like validation of a json
 * document using a linear table definition.
 *
 * Example:
 * @code
 * static const validation_section_t schema[] = {
 *   { "metadata", validate_metadata },
 *   { "header", validate_header },
 * };
 * @endcode
 */
typedef struct validation_section_s {
  /** Name of the section */
  const char *name;

  /** Validation function specific for the section */
  section_validator_t validator;
} validation_section_t;

/**
 * @brief Validate a json section using a given validator function.
 *
 * Applies a section-specific validator to a json value. It is intended to
 * be used as a building block for schema-like validation.
 *
 * @param json Pointer to the json section to validate.
 * @param validator Function pointer to the validator.
 * @return json_status_t indicating the result of the validation.
 */
json_status_t validate_section(const cJSON *json, section_validator_t validator);

/**
 * @brief Validate the complete structure of a parsed json document.
 *
 * Performs top-level validation of a json document, by calling section
 * validators for required fields.
 *
 * The json tree must have been parsed by cJSON successfully before
 * calling this function.
 *
 * The function does not take ownership of the json object and does not
 * modify it.
 *
 * @param json Pointer to the root cJSON object.
 * @return json_status_t indicating the result of the validation.
 */
json_status_t validate_json_structure(const cJSON *json);

report_data_t extract_data(const cJSON *json);

#endif /* PARSE_JSON_H */
