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

/* Unity lifecycle hooks */
void setUp(void)
{
    /* TODO */
}

void tearDown(void)
{
    /* TODO */
}

/* Optional: verify basic C runtime assumptions */
void test_basic_arithmetic(void)
{
    TEST_ASSERT_EQUAL_INT(4, 2 + 2);
}

void test_string_compare(void)
{
    TEST_ASSERT_EQUAL_STRING("unity", "unity");
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_basic_arithmetic);
    RUN_TEST(test_string_compare);

    return UNITY_END();
}
