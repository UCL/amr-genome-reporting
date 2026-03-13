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
#include "unity.h"
#include "parse_json.c"

/* Unity lifecycle hooks */
void setUp(void)
{
    /* TODO */
}

void tearDown(void)
{
    /* TODO */
}

static cJSON *parse(const char *s)
{
    cJSON *json = cJSON_Parse(s);
    if (json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: '%s'\n", error_ptr);
        }
    }
    TEST_ASSERT_NOT_NULL(json);
    return json;
}

void test_metadata_valid(void)
{
    const char *json_text =
        "{"
        "  \"author\": \"Forename Surname\","
        "  \"label\": \"Report label\","
        "  \"version\": \"v1.0\","
        "  \"timestamp\": \"2026-02-03 11:19:00\""
        "}";
    cJSON *json = parse(json_text);

    json_status_t rc = validate_metadata(json);
    TEST_ASSERT_EQUAL(OK, rc);

    cJSON_Delete(json);
}

void test_metadata_missing_author(void)
{
    const char *json_text =
        "{"
        "  \"label\": \"Report label\","
        "  \"version\": \"v1.0\","
        "  \"timestamp\": \"2026-02-03 11:19:00\""
        "}";
    cJSON *json = parse(json_text);

    json_status_t rc = validate_metadata(json);
    TEST_ASSERT_EQUAL(ERR_MISSING_FIELD, rc);

    cJSON_Delete(json);
}

void test_metadata_author_wrong_type(void)
{
    const char *json_text =
        "{"
        "  \"author\": 99,"
        "  \"label\": \"Report label\","
        "  \"version\": \"v1.0\","
        "  \"timestamp\": \"2026-02-03 11:19:00\""
        "}";
    cJSON *json = parse(json_text);

    json_status_t rc = validate_metadata(json);
    TEST_ASSERT_EQUAL(ERR_TYPE_MISMATCH, rc);

    cJSON_Delete(json);
}

void test_metadata_author_empty_string(void)
{
    const char *json_text =
        "{"
        "  \"author\": \"\","
        "  \"label\": \"Report label\","
        "  \"version\": \"v1.0\","
        "  \"timestamp\": \"2026-02-03 11:19:00\""
        "}";
    cJSON *json = parse(json_text);

    json_status_t rc = validate_metadata(json);
    TEST_ASSERT_EQUAL(ERR_INVALID_VALUE, rc);

    cJSON_Delete(json);
}

void test_header_valid(void)
{
    const char *json_text =
        "["
        "  {\"label\":\"A label\", \"value\": \"A value\"}"
        "]";
    cJSON *json = parse(json_text);

    json_status_t rc = validate_clinical_header(json);
    TEST_ASSERT_EQUAL(OK, rc);

    cJSON_Delete(json);
}

int main(void)
{
  UNITY_BEGIN();

  RUN_TEST(test_metadata_valid);
  RUN_TEST(test_metadata_missing_author);
  RUN_TEST(test_metadata_author_wrong_type);
  RUN_TEST(test_metadata_author_empty_string);

  RUN_TEST(test_header_valid);
  
  return UNITY_END();
}

