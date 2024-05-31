# Testify

Testify is a simple and effective C testing framework designed to help you manage and run tests with clear and concise reporting. This repository includes utilities for running tests on your C code, ensuring your assertions and test cases are handled efficiently.

## How to Use

To integrate and use this test runner in your project, follow these steps:

1. **Clone the Repo into Your Project**
   Clone the test runner repository into the base of your project directory:
   ```sh
   git clone https://github.com/your-repo/test-runner.git testify
   ```

2. **Append Your Makefile**
   Add the following lines to your project's Makefile to include the test runner functionality:
   ```makefile
   TEST_DIRS := src
   export TEST_DIRS

   test:
       cd testify && $(MAKE)
   ```
   Replace `src` with any directory containing test files or files containing test functions. You can include multiple directories if needed, separated by spaces.

3. **Run the Tests**
   Use the following command to run your tests:
   ```sh
   make test
   ```
## Example Output

Here is an example of the output you can expect when running Testify:

![Testify screenshot](https://github.com/conorgolden1/testify/blob/main/screenshots/output.png?raw=true)


### File Descriptions

- **test_assert.c**: Contains assertion functions used in your test cases.
- **test_assert.h**: Header file for the assertion functions.
- **test_runner.c**: Produces runner executables that will compile code with test functions and run the subsequent tests.

## Dependencies and Limitations

### Dependencies

- **Operating System:** The test runner currently works only on Linux distributions.
- **Build System:** Make sure `make` is installed on your system. You can install it using your package manager, for example:
  ```sh
  sudo apt-get install make
  ```
- **Compiler:** The default compiler used is `gcc`. You can customize the compiler by modifying the macro DEFAULT COMPILER in the `test_runner.c` file.

### Limitations

- **Operating System:** The test runner does not currently support non-Linux operating systems, such as Windows or macOS.
- **Single Compiler:** The repository is set up to use `gcc` by default. While it can be changed in the `test_runner.c` file by changing the DEFAULT COMPILER macro, there is no built-in support for other compilers out of the box.
- **File Paths:** The test runner expects relative file paths for includes and source files. Absolute paths or non-standard directory structures may cause issues.
- **Concurrency:** The test runner does not currently support running tests in parallel. Tests are executed sequentially.

These dependencies and limitations should be considered when integrating testify into your project.

### Example Project Structure

To give you a better idea, here’s an example project structure after cloning the test runner repository and modifying the Makefile:

```
your-project/
│
├── src/
│   ├── main.c
│   ├── lib/
│   │   └── lib.c
│   └── tests/
│       ├── test_main.c
│       └── test_lib.c
│
├── testify/
│   ├── Makefile
│   ├── test_assert.h
│   ├── test_assert.c
│   └── test_runner.c
│
├── Makefile
└── ...
```

In this example, the `src` directory contains the source files and the `tests` directory contains the test files. The `testify` directory contains the cloned test runner repository.

### Additional Notes

- **Test Directories:** Ensure that the `TEST_DIRS` variable in your Makefile includes all directories that contain test files.
- **Customizing the Test Runner:** You can customize the test runner by modifying any of the macros in the files in the `testify` directory.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your improvements.

## License

This project is licensed under the MIT License.
