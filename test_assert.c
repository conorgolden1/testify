
#include <stdio.h>
#include <string.h>

#define SUCCESS "\033[1;32mSUCCESS\033[0m"
#define FAILED "\033[1;31mFAILED\033[0m"
#define RESET "\033[0m"
#define MAX_OUTPUT_WIDTH 80

extern int total_tests;
extern int failed_tests;
extern int test_assertions;
extern int test_failures;

void _assert(int condition, const char *caller_name, const char *file, int line) {
    total_tests++;
    test_assertions++;
    if (!condition) {
        failed_tests++;
        test_failures++;
    }

    // Prepare the initial part of the message
    char message[256];
    snprintf(message, sizeof(message), "%s (%s:%d)", caller_name, file, line);

    // Calculate the number of dots needed
    int message_length = strlen(message);
    int dots_length = MAX_OUTPUT_WIDTH - message_length - (condition ? strlen(SUCCESS) : strlen(FAILED));

    // Create the dot string
    char dots[80];
    for (int i = 0; i < dots_length; i++) {
        dots[i] = '.';
    }
    dots[dots_length] = '\0';

    // Print the formatted message
    printf("%s%s%s\n" RESET, message, dots, condition ? SUCCESS : FAILED);
}

void _start_test(const char *test_name) {
    printf("Running test function: %s\n", test_name);
    test_assertions = 0;
    test_failures = 0;
}

void _end_test(const char *test_name) {
    printf("\nTest %s %s (%d assertions, %d passed, %d failed)\n\n",
           test_name,
           test_failures == 0 ? SUCCESS : FAILED,
           test_assertions,
           test_assertions - test_failures,
           test_failures);
}

void _print_test_summary() {
    printf("\nTest Summary:\n");
    printf("total: %d\tpassed: %d\tfailed: %d\n" RESET, total_tests,
         total_tests - failed_tests, failed_tests);
    if (failed_tests == 0) {
        printf("\x1B[32mAll tests passed!\n" RESET);
    } else {
        printf("\x1B[31mSome tests failed.\n" RESET);
    }
    printf("\n" RESET);
}


