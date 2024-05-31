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

#define DEFAULT_COMPILER "gcc"
#define BUILD_LIB_DIR "build/lib"
#define BUILD_DIR "build/tests/"
#define TEST_PREFIX ""
#define TEST_POSTFIX ".c"
#define TEST_UTILS "../../testify/"
#define RUNNER_FILENAME "runner.c"
#define RUNNER_EXECUTABLE "runner"
#define MAX_FUNCTIONS 100
#define MAX_PATH_LEN 1024

int string_ends_with(const char *str, const char *suffix) {
  int str_len = strlen(str);
  int suffix_len = strlen(suffix);

  return (str_len >= suffix_len) &&
         (0 == strcmp(str + (str_len - suffix_len), suffix));
}

char *remove_extension(const char *path) {
  char *result = (char *)malloc(strlen(path) + 1);
  if (!result) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  strcpy(result, path);
  char *dot = strrchr(result, '.');
  if (dot != NULL) {
    *dot = '\0';
  }
  return result;
}

char *get_filename_without_extension(const char *path) {
  const char *last_slash = strrchr(path, '/');
  const char *filename_start = last_slash ? last_slash + 1 : path;
  char *filename = (char *)malloc(strlen(filename_start) + 1);
  if (!filename) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  strcpy(filename, filename_start);
  char *dot = strrchr(filename, '.');
  if (dot != NULL) {
    *dot = '\0';
  }
  return filename;
}

char *resolve_include_path(const char *base_path, const char *include_path) {
  char *resolved_path = (char *)malloc(MAX_PATH_LEN);
  if (!resolved_path) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  // Copy the base path and remove the filename to get the directory
  strncpy(resolved_path, base_path, MAX_PATH_LEN);
  resolved_path[MAX_PATH_LEN - 1] = '\0'; // Ensure null termination
  char *last_slash = strrchr(resolved_path, '/');
  if (last_slash != NULL) {
    *last_slash = '\0';
  }

  // Concatenate the include path to the base directory
  strncat(resolved_path, "/", MAX_PATH_LEN - strlen(resolved_path) - 1);
  strncat(resolved_path, include_path,
          MAX_PATH_LEN - strlen(resolved_path) - 1);

  // Normalize the path (handle "../" and "./")
  char *normalized_path = (char *)malloc(MAX_PATH_LEN);
  if (!normalized_path) {
    fprintf(stderr, "Memory allocation failed\n");
    free(resolved_path);
    exit(1);
  }

  char *token;
  char *rest = resolved_path;
  int len = 0;

  while ((token = strtok_r(rest, "/", &rest))) {
    if (strcmp(token, "..") == 0) {
      // Go up one directory
      if (len > 0) {
        len--;
        while (len > 0 && normalized_path[len] != '/') {
          len--;
        }
        if (len > 0) {
          normalized_path[len] = '\0';
        }
      }
    } else if (strcmp(token, ".") != 0) {
      // Normal directory name
      if (len > 0) {
        normalized_path[len++] = '/';
      }
      strcpy(&normalized_path[len], token);
      len += strlen(token);
    }
  }
  normalized_path[len] = '\0';

  free(resolved_path);

  return normalized_path;
}

void find_test_functions(const char *test_file,
                         char functions[MAX_FUNCTIONS][256],
                         int *num_functions) {
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
      includes[*num_includes][0] = '/';
      (*num_includes)++;
    }
  }

  fclose(file);
}

int generate_runner(const char *test_file, char includes[MAX_FUNCTIONS][256],
                     int num_includes) {
  FILE *runner = fopen(BUILD_DIR RUNNER_FILENAME, "w");
  if (runner == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  char functions[MAX_FUNCTIONS][256];
  int num_functions;
  find_test_functions(test_file, functions, &num_functions);
  if (num_functions == 0) {
    return num_functions;
  }
  fprintf(runner, "#include <stdio.h>\n");
  for (int i = 0; i < num_includes; i++) {
    char *path = resolve_include_path(test_file, includes[i]);
    fprintf(runner, "#include \"../../%s\n", path);
    free(path);
  }
  fprintf(runner, "extern int __test_runner__;\n");

  fprintf(runner, "void run_all_tests() {\n");
  for (int i = 0; i < num_functions; i++) {
    fprintf(runner, "    _start_test(\"%s\");\n", functions[i]);
    fprintf(runner, "    %s();\n", functions[i]);
    fprintf(runner, "    _end_test(\"%s\");\n", functions[i]);
  }
  fprintf(runner, "}\n\n");

  fprintf(runner, "int test_main() {\n");
  fprintf(runner, "    __test_runner__ = 1;\n");
   /* line where GCC complains about implicit function declaration */
  fprintf(runner, "    run_all_tests();\n");
  fprintf(runner, "    return 0;\n");
  fprintf(runner, "}\n");

  fclose(runner);
  return num_functions;
}

void compile_and_run(const char *test_file, char includes[MAX_FUNCTIONS][256],
                     int num_includes) {
  char compile_cmd[2048];
  char object_files[1024] = "";

  // Create the Makefile path
  char makefile_path[512];
  snprintf(makefile_path, sizeof(makefile_path), "%sMakefile", BUILD_DIR);

  // Open the Makefile for writing
  FILE *makefile = fopen(makefile_path, "w");
  if (makefile == NULL) {
    perror("Failed to create Makefile");
    return;
  }

  // Write the initial part of the Makefile
  fprintf(makefile, "CC=%s\nCFLAGS=-w -std=c11\n\n",
          DEFAULT_COMPILER);
  fprintf(makefile, "all:\n");
  // Compile the included files
  for (int i = 0; i < num_includes; i++) {
    char *path = resolve_include_path(test_file, includes[i]);
    char *no_ext = remove_extension(path);
    char *filename = get_filename_without_extension(path);

    snprintf(compile_cmd, sizeof(compile_cmd),
             "\t$(CC) $(CFLAGS) -c ../../%s.c -o %s.o\n", no_ext, filename);
    fprintf(makefile, "%s", compile_cmd);
    snprintf(compile_cmd, sizeof(compile_cmd), "%s.o ", filename);
    strcat(object_files, compile_cmd);

    free(no_ext);
    free(filename);
    free(path);
  }

  // Compile the test file
  snprintf(compile_cmd, sizeof(compile_cmd),
           "\t$(CC) $(CFLAGS) -c ../../%s -o test_file.o\n", test_file);
  fprintf(makefile, "%s", compile_cmd);
  strcat(object_files, "test_file.o ");

  // Compile the object files in the build/lib directory
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

  // Link all the object files
  snprintf(compile_cmd, sizeof(compile_cmd),
           "\t$(CC) $(CFLAGS) -Wl,--defsym=main=test_main  %s %s -o %s\n", object_files,
           RUNNER_FILENAME, RUNNER_EXECUTABLE);
  fprintf(makefile, "%s", compile_cmd);

  // Running the tests
  fprintf(makefile, "\t./%s\n", RUNNER_EXECUTABLE);

  // Close the Makefile
  fclose(makefile);

  // Run the make command in the BUILD_DIR
  char make_command[512];
  snprintf(make_command, sizeof(make_command), "make -C %s", BUILD_DIR);
  printf("%s\n", make_command);
  if (system(make_command) != 0) {
    fprintf(stderr, "Compilation failed\n");
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
        char includes[MAX_FUNCTIONS][256];
        int num_includes;
        find_includes(test_file_path, includes, &num_includes);
        if (generate_runner(test_file_path, includes, num_includes) == 0) {
            continue;
        }
        compile_and_run(test_file_path, includes, num_includes);
      }
    }
  }
  closedir(dir);
  return 0;
}
