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

typedef int (*section_validator_t)(const cJSON *section);

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

int validate_json_structure(const cJSON *json);

json_status_t validate_section(const cJSON *json, section_validator_t validator);

#endif /* PARSE_JSON_H */
