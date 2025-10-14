# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## About Qucs-S

Qucs-S is a graphical circuit simulator with support for multiple SPICE simulation kernels (Ngspice, Xyce, SpiceOpus) and the native Qucsator engine. It's a Qt6-based C++ application with extensive component libraries and visualization capabilities.

## Build System

### Building
All builds use CMake with out-of-source builds required:

```bash
git submodule init
git submodule update
mkdir builddir
cd builddir
cmake .. -DCMAKE_INSTALL_PREFIX=/your_install_prefix/
make
make install
```

### Build Options

#### Simulator Backend Configuration
- **`WITH_NGSPICE_SHARED`** (default: OFF) - Enable ngspice shared library support
  - When enabled, simulations use ngspice as a shared library (libngspice) instead of external process
  - Requires libngspice development package installed
  - Example: `cmake .. -DWITH_NGSPICE_SHARED=ON`
  - Install on macOS: `brew install libngspice`
  - Install on Ubuntu: `sudo apt-get install libngspice0-dev`

- **`WITH_EXTERNAL_SIMULATORS`** (default: ON) - Enable external simulator support (QProcess-based)
  - Traditional mode: runs simulators (ngspice, xyce, etc.) as external processes
  - Can be disabled if only using shared library mode
  - Example: `cmake .. -DWITH_EXTERNAL_SIMULATORS=OFF`

Both options can be enabled simultaneously. Priority order: shared library → external process.

### Dependencies
- **Required**: Qt6 (Core, Gui, Widgets, Svg, SvgWidgets, Xml, PrintSupport, Charts), CMake, gperf, dos2unix, C++20 compiler
- **Runtime**: ngspice (primary simulation kernel, or libngspice if using shared library mode)
- **Optional**: libngspice (for WITH_NGSPICE_SHARED), flex/bison (only for QucsatorRF submodule)

### Running
```bash
cd /your_install_prefix/bin
./qucs-s
```

### Testing
Test suite uses Qt Test framework and CTest:
```bash
cd builddir
ctest                           # Run all tests
ctest --output-on-failure       # Show output only for failing tests
ctest --verbose                 # Show all output
```

Individual test executables (from builddir):
- `tests/test_ngspice_integration` - Ngspice shared library integration tests
- `tests/test_ngspice_kernel` - AbstractSpiceKernel and SimulatorError enum tests
- `tests/test_simulation_types` - DC, AC, and Transient simulation type tests
- `qucs/geometry/test_geometry` - Geometry library unit tests

Tests run automatically in CI via GitHub Actions (including xvfb on Linux for UI tests).

## Code Organization

### Main Application (`qucs/`)
- **qucs.cpp/h**: Main application window (QucsApp class)
- **schematic.cpp/h**: Schematic editor canvas and logic
- **main.cpp**: Application entry point
- **mouseactions.cpp**: Mouse interaction handling for schematic editor

### Component System (`qucs/components/`)
Contains 300+ component implementations. Each component typically has:
- `.cpp` file with component logic and properties
- `.h` file with class definition
Components inherit from base Element class

### Simulation Kernels (`qucs/extsimkernels/`)
- **abstractspicekernel.cpp/h**: Base class for SPICE kernel interfaces
  - Conditionally compiles QProcess code based on WITH_EXTERNAL_SIMULATORS
  - Provides common netlist generation and output parsing
- **ngspice.cpp/h**: Ngspice integration (primary kernel)
  - Supports both external process (QProcess) and shared library modes
  - Automatically selects mode based on build configuration
- **ngspice_shared.cpp/h**: Ngspice shared library wrapper (when WITH_NGSPICE_SHARED enabled)
  - Interfaces with libngspice via C API
  - Implements callbacks for output, status, and data retrieval
  - Emits Qt signals to integrate with existing architecture
- **xyce.cpp/h**: Xyce simulator support
- **spicecompat.cpp/h**: SPICE compatibility layer
- **qucs2spice.cpp/h**: Library model format converter (converts Qucs-format models to SPICE format for library components)

### Visualization (`qucs/diagrams/`)
- **diagram.cpp/h**: Base diagram/graph classes
- **graph.cpp/h**: Data plotting
- **marker.cpp/h**: Graph markers for measurements
- **curvediagram.h**, **polardiagram.h**, etc.: Specialized plot types

### Dialogs (`qucs/dialogs/`)
Qt dialog implementations for settings, component properties, import/export, etc.

### Utility Tools
Separate GUI applications built alongside main app:
- **qucs-filter/**: Filter design tool
- **qucs-attenuator/**: Attenuator calculator
- **qucs-transcalc/**: Transmission line calculator
- **qucs-activefilter/**: Active filter design
- **qucs-powercombining/**: Power combining calculator
- **qucs-s-spar-viewer/**: S-parameter viewer

### Libraries (`library/`)
SPICE model libraries (`.lib` files) for various component types: BJT, MOSFETs, diodes, ICs, etc.

### Geometry System (`qucs/geometry/`)
Header-only geometry library for shapes and points. Has unit tests.

### Test Suite (`tests/`)
Qt Test-based test suite with CTest integration:
- **test_ngspice_integration.cpp**: Integration tests for NgspiceShared class
  - Tests initialization, circuit loading, simulation execution
  - Validates Qt signal emission (outputReceived, simulationStarted)
  - Tests vector data retrieval from simulation results
  - Only runs when WITH_NGSPICE_SHARED=ON
- **test_ngspice_kernel.cpp**: Unit tests for AbstractSpiceKernel
  - Tests SimulatorError enum values and error handling
  - Validates background command rejection in shared library mode
  - Tests error signal emission
- **test_simulation_types.cpp**: Comprehensive simulation type testing
  - Tests DC operating point analysis (.op)
  - Tests AC frequency sweep analysis (.ac) with RC filters
  - Tests transient time-domain analysis (.tran) with RC step response
  - Validates voltage dividers, frequency response, and time constants
  - Verifies vector data (time, frequency, voltage) retrieval
  - Only runs when WITH_NGSPICE_SHARED=ON
- **CMakeLists.txt**: Test configuration with labels ("unit", "integration")

## Code Style

### C++ Standards
- **C++20**: Standard for new code
- **Qt6**: All Qt code uses Qt6 APIs
- **Modern C++**: Prefer C++17/20 features over legacy approaches

### Naming Conventions
- **Variables/functions**: `camelCaseWithSmallFirstLetter`
- **Classes**: CapitalizedCamelCase (Qt style)

### Formatting
- Use `clang-format` for all changes (config in `.clang-format`)
- Format specific lines: `clang-format --lines=<start>:<end> path/to/file`
- Key settings: 80 column limit, 2-space indent, pointer-left (`int* ptr`), braces on same line
- **Always use braces** for `if`, `for`, `while`, etc., even for single statements

### Required Patterns
```cpp
// Bad - forbidden
if (condition)
  doSomething();

// Good - required
if (condition) {
  doSomething();
}
```

## Git Workflow

### Branch Strategy
- **`current`**: Main development branch (use for PRs with new features)
- **`release/YY.N`**: Release branches (use for bugfix PRs)
- **`master`**: Stable releases

### Commit Messages
Format (max 80 columns):
```
Brief description on first line

Optional detailed description after blank line.
Can span multiple lines.
```

Examples from history:
- `fix(WireLabel::mirrorX/Y): Handle axis input`
- `Fix Monte Carlo ngspice template`

### Pull Requests
- **Bugfixes**: Base on `release/YY.N`, prefix branch with issue number (e.g., `310-fix-window-size`)
- **Features**: Base on `current`
- One logical unit per PR (single bugfix or feature)
- Atomic commits, easy to review
- PRs must merge without conflicts

## CI/CD

GitHub Actions workflow (`.github/workflows/deploy.yml`):
- Builds on Linux (AppImage, Qt6)
- Builds on push to `master`, `current`, `release/*` branches and tags
- Check status: `gh pr checks` or `gh pr checks --watch`

## LSP/IDE Support

### clangd Setup
`compile_commands.json` generated by CMake in `builddir/`:
```bash
cd builddir
cmake ..
cd ..
ln -s ./builddir/compile_commands.json compile_commands.json
```

Configuration in `.clangd` file.

## Project-Specific Notes

### Qt Integration
- Uses Qt `.ui` files for some dialogs (auto-compiled by CMake via `CMAKE_AUTOUIC`)
- Qt MOC enabled (`CMAKE_AUTOMOC`)
- Resource files (`.qrc`) for bitmaps/icons

### Simulation Flow
1. User creates schematic in GUI
2. Schematic components generate SPICE netlist
3. Netlist sent to simulation kernel (typically ngspice via shared library or external process)
4. Results parsed and visualized in diagrams

### Submodules
- `qucsator_rf/` is a git submodule containing the RF circuit simulator
- Must run `git submodule init && git submodule update` after clone to populate it

## Common Tasks

### Adding a New Component
1. Create `.cpp` and `.h` files in `qucs/components/`
2. Inherit from base `Component` class
3. Implement properties, SPICE conversion, and visual representation
4. Add to component registry in components system

### Debugging Build Issues
- Check component library files in `library/` are installed
- Verify Qt6 development packages present
- Ensure gperf is available
- Review CMake output for missing dependencies

### Working with Schematic Editor
Key interaction code in `mouseactions.cpp` and `schematic.cpp`.

### Translation Updates
Run with `-DUPDATE_TRANSLATIONS=ON` to update `.ts` translation files from source.

### Adding Tests
1. Create test file in `tests/` directory (e.g., `test_newfeature.cpp`)
2. Use Qt Test framework (`#include <QTest>`, `QTEST_MAIN`, `QSignalSpy`)
3. Add test executable and configuration to `tests/CMakeLists.txt`:
   ```cmake
   add_executable(test_newfeature test_newfeature.cpp)
   target_link_libraries(test_newfeature PRIVATE Qt6::Test extsimkernels)
   set_target_properties(test_newfeature PROPERTIES AUTOMOC ON)
   add_test(NAME NewFeature COMMAND test_newfeature)
   set_tests_properties(NewFeature PROPERTIES TIMEOUT 30 LABELS "unit")
   ```
4. Build and run: `cd builddir && cmake .. && make && ctest -R NewFeature`
5. Tests run automatically in CI on all platforms

Test guidelines:
- Use descriptive test names (e.g., `testNgspiceSharedInitialization`)
- Add `qDebug()` statements for diagnostic output
- Use `QVERIFY2` with descriptive messages for better failure reporting
- Set appropriate timeouts (default: 30s for unit tests, 60s for simulations)
- Use conditional compilation (`#if NGSPICE_SHARED`) for feature-specific tests
- Clean up resources in `cleanupTestCase()` to avoid test pollution

### Testing Simulation Types
When adding tests for simulation blocks (DC, AC, Transient, etc.):
1. Use raw SPICE netlist format (not schematic parsing)
2. Structure netlist as QStringList with proper SPICE syntax:
   - Title line (first line)
   - Component definitions (V1, R1, C1, etc.)
   - Simulation directive (.op, .ac, .tran)
   - .end statement
3. Example AC test netlist:
   ```cpp
   QStringList netlist;
   netlist << "AC Frequency Test";
   netlist << "V1 in 0 DC 0 AC 1";     // AC source
   netlist << "R1 in out 1k";
   netlist << "C1 out 0 1u";
   netlist << ".ac lin 50 1 100k";     // Linear sweep, 50 points, 1Hz to 100kHz
   netlist << ".end";
   ```
4. Wait for simulation to complete using QEventLoop or QTest::qWait()
5. Retrieve and validate vectors using `currentPlot()` and `allVectors()`
6. Check for expected vectors: frequency, time, voltage nodes, currents
7. Verify vector data is reasonable for the circuit being tested

See `tests/test_simulation_types.cpp` for comprehensive examples of DC, AC, and Transient tests.
