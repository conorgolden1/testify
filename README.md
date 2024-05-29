# Testify

Testify is a simple and effective C testing framework designed to help you manage and run tests with clear and concise reporting. This repository includes utilities for running tests on your C code, ensuring your assertions and test cases are handled efficiently.

## File Structure

```
.test_utils/
    test_assert.c
    test_assert.h
    test_runner.c
```

### File Descriptions

- **test_assert.c**: Contains assertion functions used in your test cases.
- **test_assert.h**: Header file for the assertion functions.
- **test_runner.c**: Manages and runs the test cases, providing output for each test's success or failure.

## Example Output

Here is an example of the output you can expect when running Testify:

![Testify screenshot](https://github.com/conorgolden1/testify/blob/main/screenshots/output.png?raw=true)

## How to Use

1. **Clone the Repository**:
    ```sh
    git clone https://github.com/yourusername/testify.git
    cd testify
    ```

2. **Compile the Test Runner**:
    ```sh
    gcc -o test_runner .test_utils/test_runner.c .test_utils/test_assert.c -I.test_utils
    ```

3. **Run Your Tests**:
    ```sh
    ./test_runner path/to/your/test_file.c
    ```

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your improvements.

## License

This project is licensed under the MIT License.
