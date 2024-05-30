#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SRC_DIR "src"
#define BUILD_LIB_DIR "build/lib"
#define BUILD_DIR "build/tests/"
#define TEST_PREFIX ""
#define TEST_POSTFIX ".c"
#define TEST_UTILS "../../testify/"
#define RUNNER_FILENAME BUILD_DIR "runner.c"
#define RUNNER_EXECUTABLE BUILD_DIR "runner"
#define MAX_FUNCTIONS 100

int string_ends_with(const char *str, const char *suffix) {
  int str_len = strlen(str);
  int suffix_len = strlen(suffix);

  return (str_len >= suffix_len) &&
         (0 == strcmp(str + (str_len - suffix_len), suffix));
}

void find_test_functions(const char *test_file,
                         char functions[MAX_FUNCTIONS][256],
                         int *num_functions) {
  printf("Searching test functions:\t%s\n", test_file);
  FILE *file = fopen(test_file, "r");
  if (!file) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  char line[256];
  *num_functions = 0;
  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "void test", 9) == 0) {
      sscanf(line, "void %s", functions[*num_functions]);
      // Remove the trailing parenthesis
      char *paren = strchr(functions[*num_functions], '(');
      if (paren)
        *paren = '\0';
      (*num_functions)++;
    }
  }

  fclose(file);
}

void find_includes(const char *test_file, char includes[MAX_FUNCTIONS][256],
                   int *num_includes) {
  FILE *file = fopen(test_file, "r");
  if (!file) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  char line[256];
  *num_includes = 0;
  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "#include \"", 8) == 0 &&
        string_ends_with(line, ".h\"\n")) {
      sscanf(line, "#include %s", includes[*num_includes]);
      // Remove the trailing newline or any quotes/angle brackets
      includes[*num_includes][strcspn(includes[*num_includes], "\n")] = 0;
      (*num_includes)++;
    }
  }

  fclose(file);
}

void generate_runner(const char *test_file) {
  FILE *runner = fopen(RUNNER_FILENAME, "w");
  if (runner == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  char functions[MAX_FUNCTIONS][256];
  int num_functions;
  find_test_functions(test_file, functions, &num_functions);

  char includes[MAX_FUNCTIONS][256];
  int num_includes;
  find_includes(test_file, includes, &num_includes);

  fprintf(runner, "#include <stdio.h>\n");
  for (int i = 0; i < num_includes; i++) {
    includes[i][0] = '/';
    fprintf(runner, "#include \"..%s\n", includes[i]);
  }
  fprintf(runner, "int test_assertions = 0;\n");
  fprintf(runner, "int test_failures = 0;\n");
  fprintf(runner, "int total_tests = 0;\n");
  fprintf(runner, "int failed_tests = 0;\n\n");

  fprintf(runner, "void run_all_tests() {\n");
  fprintf(runner, "    #pragma GCC diagnostic push\n");
  fprintf(runner, "    #pragma GCC diagnostic ignored "
                  "\"-Wimplicit-function-declaration\"\n");
  // Write function calls
  for (int i = 0; i < num_functions; i++) {
    fprintf(runner, "    _start_test(\"%s\");\n", functions[i]);
    fprintf(runner, "    %s();\n", functions[i]);
    fprintf(runner, "    _end_test(\"%s\");\n", functions[i]);
  }

  fprintf(runner, "    #pragma GCC diagnostic pop\n");
  fprintf(runner, "}\n\n");

  fprintf(runner, "int test_main() {\n");
  /* line where GCC complains about implicit function declaration */
  fprintf(runner, "    run_all_tests();\n");
  fprintf(runner, "    #pragma GCC diagnostic push\n");
  fprintf(runner, "    #pragma GCC diagnostic ignored "
                  "\"-Wimplicit-function-declaration\"\n");
  fprintf(runner, "    _print_test_summary();\n");
  fprintf(runner, "    #pragma GCC diagnostic pop\n");
  fprintf(runner, "    return 0;\n");
  fprintf(runner, "}\n");

  fclose(runner);
}

void compile_and_run(const char *test_file) {
  char compile_cmd[2048];
  snprintf(compile_cmd, sizeof(compile_cmd),
           "gcc -c ./testify/test_assert.c -o %stest_assert.o", BUILD_DIR);
  printf("%s\n", compile_cmd);
  system(compile_cmd);

  snprintf(compile_cmd, sizeof(compile_cmd), "gcc  -I. -c %s -o %stest_file.o",
           test_file, BUILD_DIR);
  printf("%s\n", compile_cmd);

  system(compile_cmd);

  char object_files[1024] = "";
  struct dirent *entry;
  DIR *lib_dir = opendir(BUILD_LIB_DIR);
  if (lib_dir) {
    while ((entry = readdir(lib_dir)) != NULL) {
      if (strstr(entry->d_name, ".o")) {
        char object_file_path[512];
        snprintf(object_file_path, sizeof(object_file_path), "%s/%s",
                 BUILD_LIB_DIR, entry->d_name);
        strcat(object_files, object_file_path);
        strcat(object_files, " ");
      }
    }
    closedir(lib_dir);
  }

  snprintf(
      compile_cmd, sizeof(compile_cmd),
      "gcc -Wl,--defsym=main=test_main -I. %stest_file.o %stest_assert.o %s "
      "%s -o %s",
      BUILD_DIR, BUILD_DIR, object_files, RUNNER_FILENAME, RUNNER_EXECUTABLE);
  printf("%s\n", compile_cmd);

  if (system(compile_cmd) != 0) {
    fprintf(stderr, "Compilation failed\n");
    return;
  }

  printf("Running tests in %s\n", test_file);
  printf("./%s\n", RUNNER_EXECUTABLE);
  if (system("./" RUNNER_EXECUTABLE) != 0) {
    fprintf(stderr, "Tests failed\n");
    return;
  }
}

int main(int argc, char *argv[]) {
  DIR *dir;
  struct dirent *entry;

  if (argc < 2) {
    fprintf(stderr,
            "Usage: export SRC_DIRS in makefile that calls testify makefile\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 1; i < argc; i++) {
    if ((dir = opendir(argv[i])) == NULL) {
      perror(argv[i]);
      continue;
    }

    mkdir(BUILD_DIR, 0777);

    while ((entry = readdir(dir)) != NULL) {
      if (strncmp(entry->d_name, TEST_PREFIX, strlen(TEST_PREFIX)) == 0 &&
          string_ends_with(entry->d_name, ".c")) {
        char test_file_path[512];
        snprintf(test_file_path, sizeof(test_file_path), "%s/%s", argv[i],
                 entry->d_name);

        generate_runner(test_file_path);
        compile_and_run(test_file_path);
      }
    }
  }
  closedir(dir);
  return 0;
}
