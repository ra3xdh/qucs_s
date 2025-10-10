# Qucs-S UI Regression Test Suite

enable_testing()

# Find Qt6 Test module
find_package(Qt6 COMPONENTS Test REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/qucs
    ${CMAKE_SOURCE_DIR}/qucs/extsimkernels
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/qucs
)

# Test: NgspiceIntegration
# Tests ngspice shared library integration and signals
add_executable(test_ngspice_integration test_ngspice_integration.cpp)
target_link_libraries(test_ngspice_integration
    PRIVATE
    Qt6::Test
    Qt6::Core
    extsimkernels
)

# Enable Qt Test automoc
set_target_properties(test_ngspice_integration PROPERTIES AUTOMOC ON)

# Add to CTest
add_test(NAME NgspiceIntegration COMMAND test_ngspice_integration)

# Set test properties
set_tests_properties(NgspiceIntegration PROPERTIES
    TIMEOUT 30
    LABELS "integration"
)

# Test: NgspiceKernel
# Tests Ngspice class and AbstractSpiceKernel functionality
add_executable(test_ngspice_kernel test_ngspice_kernel.cpp)
target_link_libraries(test_ngspice_kernel
    PRIVATE
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    extsimkernels
)

# Enable Qt Test automoc
set_target_properties(test_ngspice_kernel PROPERTIES AUTOMOC ON)

# Add to CTest
add_test(NAME NgspiceKernel COMMAND test_ngspice_kernel)

# Set test properties
set_tests_properties(NgspiceKernel PROPERTIES
    TIMEOUT 30
    LABELS "unit"
)

message(STATUS "UI Regression test suite configured")
if(NGSPICE_SHARED)
    message(STATUS "  - NgspiceIntegration test: ENABLED (with shared library support)")
    message(STATUS "  - NgspiceKernel test: ENABLED")
else()
    message(STATUS "  - NgspiceIntegration test: ENABLED (without shared library support)")
    message(STATUS "  - NgspiceKernel test: ENABLED")
endif()
