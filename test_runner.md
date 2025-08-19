# PEARL Firmware Test Runner

This is a simple test runner for the PEARL project, designed to run on the Pico and output results over serial (USB UART).

## How it works
- Add test functions for your firmware logic, menu, and LCD code.
- The test runner will call each test and print PASS/FAIL to the serial port.

## Example usage
- Build and flash this test runner to your Pico.
- Open a serial terminal (e.g., VS Code Serial Monitor, PuTTY, or Web Serial in your browser).
- View the test results.

## Next steps
- Add more tests for each function you want to verify.
- Optionally, extend the test runner to accept commands over serial for interactive or automated testing.
