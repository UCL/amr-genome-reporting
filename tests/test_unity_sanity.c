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
