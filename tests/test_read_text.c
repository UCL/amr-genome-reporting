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
#include "read_text.h"

#include <stdlib.h>
#include <string.h>

/* Unity lifecycle hooks */
void setUp(void)
{
    /* TODO */
}

void tearDown(void)
{
    /* TODO */
}

struct memfile {
  FILE *fp;
  char *buf;
};

static struct memfile open_mem_stream_from_string(const char *s)
{
  struct memfile m = {0};

#if defined(__APPLE__) || defined(__linux__)
  size_t len = strlen(s);
  if (len == 0) {
    m.fp = tmpfile();
    TEST_ASSERT_NOT_NULL(m.fp);
  } else {
    m.buf = malloc(len);
    TEST_ASSERT_NOT_NULL(m.buf);
    memcpy(m.buf, s, len);
    m.fp = fmemopen(m.buf, len, "r");
    TEST_ASSERT_NOT_NULL(m.fp);
  }
#else
  /* Write to temp file */
  m.fp = tmpfile();
  TEST_ASSERT_NOT_NULL(m.fp);
  fputs(s, m.fp);
  rewind(m.fp);
#endif

  return m;
}

void test_empty_file(void)
{
  struct memfile m = open_mem_stream_from_string("");
  size_t len = 123;

  char *buf = read_text_file(m.fp, &len);

  TEST_ASSERT_NOT_NULL(buf);
  TEST_ASSERT_EQUAL_UINT64(0, (uint64_t)len);
  TEST_ASSERT_EQUAL_STRING("", buf);

  free(buf);
  fclose(m.fp);
  free(m.buf);
}

void test_single_line_json(void)
{
  const char *json = "{\"a\":1,\"b\":2}";
  struct memfile m = open_mem_stream_from_string(json);
  size_t len = 0;
  
  char *buf = read_text_file(m.fp, &len);

  TEST_ASSERT_NOT_NULL(buf);
  TEST_ASSERT_EQUAL_UINT64(strlen(json), (uint64_t)len);
  TEST_ASSERT_EQUAL_STRING(json, buf);

  free(buf);
  fclose(m.fp);
  free(m.buf);  
}

void test_multi_line_json(void)
{
  const char *json =
    "{\n"
    "  \"a\": 1,\n"
    "  \"b\": 2\n"
    "}\n";
  struct memfile m = open_mem_stream_from_string(json);
  size_t len = 0;
  
  char *buf = read_text_file(m.fp, &len);

  TEST_ASSERT_NOT_NULL(buf);
  TEST_ASSERT_EQUAL_UINT64(strlen(json), (uint64_t)len);
  TEST_ASSERT_EQUAL_STRING(json, buf);

  free(buf);
  fclose(m.fp);
  free(m.buf);  
}

void test_long_line(void)
{
  char input[8182];
  memset(input, 'x', sizeof(input) - 1);
  input[sizeof(input) - 1] = '\0';
  struct memfile m = open_mem_stream_from_string(input);
  size_t len = 0;
  
  char *buf = read_text_file(m.fp, &len);

  TEST_ASSERT_NOT_NULL(buf);
  TEST_ASSERT_EQUAL_UINT64(strlen(input), (uint64_t)len);
  TEST_ASSERT_EQUAL_STRING(input, buf);

  free(buf);
  fclose(m.fp);
  free(m.buf);
}

int main(void)
{
  UNITY_BEGIN();

  RUN_TEST(test_empty_file);
  RUN_TEST(test_single_line_json);
  RUN_TEST(test_multi_line_json);
  RUN_TEST(test_long_line);
  
  return UNITY_END();
}
