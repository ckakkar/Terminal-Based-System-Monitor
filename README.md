# TBM - Terminal-Based System Monitor

A modern, feature-rich terminal-based system monitor built with C++ and ftxui. TBM provides real-time monitoring of CPU usage, memory consumption, and process statistics with a beautiful text-based user interface.

## Features

- **Real-time System Monitoring**
  - CPU usage with visual bar graphs
  - Memory usage (total, used, free, cached)
  - Per-process CPU and memory statistics

- **Process Management**
  - Live process list with detailed information
  - Sortable by CPU, memory, PID, or name
  - Process filtering with fuzzy search

- **Fuzzy Search**
  - Levenshtein distance algorithm for intelligent process filtering
  - Case-insensitive matching
  - Substring and approximate matching

- **Modern TUI**
  - Built with [ftxui](https://github.com/ArthurSonzogni/FTXUI) for a modern terminal UI
  - Responsive and asynchronous updates
  - Keyboard shortcuts for navigation

- **Cross-Platform**
  - Linux (uses `/proc` filesystem)
  - macOS (uses `sysctl` and Mach APIs)

## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.20 or higher
- vcpkg for dependency management
- Git

## Building

### Prerequisites

1. **Install vcpkg** (if not already installed):
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh  # On macOS/Linux
   # or
   .\bootstrap-vcpkg.bat  # On Windows
   ```

2. **Set vcpkg toolchain** (recommended):
   ```bash
   export VCPKG_ROOT=/path/to/vcpkg
   ```

### Build Steps

1. **Clone the repository**:
   ```bash
   git clone <repository-url>
   cd TBM
   ```

2. **Configure with CMake**:
   ```bash
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
   ```

   On Windows:
   ```cmd
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
   ```

3. **Build the project**:
   ```bash
   cmake --build build --config Release
   ```

4. **Run the application**:
   ```bash
   ./build/TBM  # On macOS/Linux
   # or
   .\build\Release\TBM.exe  # On Windows
   ```

## Running Tests

After building, run the test suite:

```bash
cd build
ctest --output-on-failure
```

Or run the test executable directly:

```bash
./build/tests
```

## Usage

### Keyboard Shortcuts

- `/` - Focus search input to filter processes
- `q` or `ESC` - Quit the application
- `F1` - Toggle help screen
- `↑/↓` - Navigate process list (when implemented)

### Process Filtering

Type in the search box to filter processes by name. The fuzzy search algorithm will match:
- Exact matches
- Substring matches
- Similar names (using Levenshtein distance)

Examples:
- Type `chr` to find `chromium`
- Type `fire` to find `firefox`
- Type `code` to find `code` or similar processes

## Project Structure

```
TBM/
├── CMakeLists.txt          # Main CMake configuration
├── vcpkg.json              # vcpkg dependencies
├── include/                # Header files
│   ├── system_monitor.hpp
│   ├── process_manager.hpp
│   ├── fuzzy_search.hpp
│   ├── tui.hpp
│   ├── linux_monitor.hpp
│   └── macos_monitor.hpp
├── src/                    # Source files
│   ├── main.cpp
│   ├── system_monitor.cpp
│   ├── process_manager.cpp
│   ├── fuzzy_search.cpp
│   ├── tui.cpp
│   ├── linux_monitor.cpp
│   └── macos_monitor.cpp
├── tests/                  # Unit tests
│   ├── CMakeLists.txt
│   ├── test_fuzzy_search.cpp
│   ├── test_process_manager.cpp
│   └── test_system_monitor.cpp
└── .github/
    └── workflows/
        └── ci.yml          # GitHub Actions CI/CD
```

## Dependencies

- **ftxui**: Modern C++ terminal UI library
- **Google Test**: Unit testing framework

All dependencies are managed through vcpkg and will be automatically downloaded and built during the CMake configuration step.

## CI/CD

The project includes a GitHub Actions workflow (`.github/workflows/ci.yml`) that:
- Builds the project on Linux and macOS
- Runs the test suite
- Validates the build on every push and pull request

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is open source. Please check the license file for details.

## Acknowledgments

- Built with [ftxui](https://github.com/ArthurSonzogni/FTXUI)
- Inspired by `htop` and other system monitoring tools

