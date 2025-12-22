# sid-json Unit Tests

This directory contains comprehensive unit tests for the sid-json library using Google Test framework.

## Test Files

- `test_main.cpp` - Test runner and main entry point
- `test_value.cpp` - Tests for JSON value operations (types, conversions, containers)
- `test_parser.cpp` - Tests for JSON parsing (objects, arrays, errors, duplicate keys)
- `test_format.cpp` - Tests for JSON output formatting (compact, pretty, escaping)

## Prerequisites

Install Google Test:
```bash
# Ubuntu/Debian
sudo apt-get install libgtest-dev

# macOS with Homebrew
brew install googletest

# Or build from source
git clone https://github.com/google/googletest.git
cd googletest && mkdir build && cd build
cmake .. && make && sudo make install
```

## Building and Running Tests

```bash
# From project root
mkdir build && cd build
cmake -DBUILD_TESTING=ON ..
make

# Run all tests
make test

# Run with verbose output
ctest --verbose

# Run specific test executable directly
./tests/sid-json-tests
```

## Test Coverage

### Value Tests
- Type checking and conversions
- Array and object operations
- Copy and assignment operations
- Default constructor behavior

### Parser Tests
- Basic JSON parsing (objects, arrays, primitives)
- Nested structure parsing
- Error handling for invalid JSON
- Duplicate key handling modes
- Comment parsing support
- Escaped string handling

### Format Tests
- Compact vs pretty formatting
- Custom indentation settings
- Nested structure formatting
- Special value formatting (null, boolean)
- String escaping in output
