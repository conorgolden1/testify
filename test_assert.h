
#ifndef TEST_ASSERT_H
#define TEST_ASSERT_H

#define assert(condition) _assert((condition), __func__, __FILE__, __LINE__)

void _assert(int condition, const char *caller_name, const char *file, int line);

void _start_test(const char *test_name);

void _end_test(const char *test_name);

void _print_test_summary();

#endif // TEST_ASSERT_H

