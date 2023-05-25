PROJECT(qucs-suite CXX)
cmake_minimum_required(VERSION 3.10)
cmake_policy(VERSION 3.10)

set(QUCS_NAME "qucs-s")

# Prohibit in-source builds
if("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(
    FATAL_ERROR
      "QUCS-S: You cannot build in a source directory (or any directory with "
      "CMakeLists.txt file). Please make a build subdirectory. Feel free to "
      "remove CMakeCache.txt and CMakeFiles.")
endif()

file (STRINGS "${qucs-suite_SOURCE_DIR}/VERSION" QUCS_VERSION)
message(STATUS "Configuring Qucs: VERSION ${QUCS_VERSION}")

set(GIT unknown)
if(EXISTS ${CMAKE_SOURCE_DIR}/../.git )
  find_package(Git)
  # Get the latest abbreviated commit hash of the working branch
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log --pretty=format:%h -n 1u
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
  )
  set(GIT ${GIT_COMMIT_HASH})
  message(STATUS "Found Git repository, last commit hash: ${GIT}")
endif()

message(STATUS "${PROJECT_NAME} ${CMAKE_INSTALL_PREFIX} ${qucs-suite_BINARY_DIR}" )

set(QT_DEFAULT_MAJOR_VERSION 6)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets LinguistTools)
message(STATUS "QT Major Version: " ${QT_VERSION_MAJOR})

add_definitions(${QT_DEFINITIONS})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(QT_NO_DEBUG_OUTPUT)
endif()

add_subdirectory( qucs )
add_subdirectory( converter )
add_subdirectory( qucs-activefilter )
add_subdirectory( qucs-attenuator )
#add_subdirectory( qucs-doc )
add_subdirectory( qucs-filter )
add_subdirectory( library )
add_subdirectory( qucs-transcalc )
add_subdirectory( qucs-powercombining )
#add_subdirectory( examples )
add_subdirectory( translations )

install(DIRECTORY "examples" DESTINATION "share/${QUCS_NAME}")

#
# Custom uninstall target
#
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)


