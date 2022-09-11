PROJECT(qucs-project CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
cmake_policy(VERSION 3.0)

SET(QUCS_NAME "qucs-s")

file (STRINGS "${qucs-project_SOURCE_DIR}/VERSION" QUCS_VERSION)
message(STATUS "Configuring Qucs: VERSION ${QUCS_VERSION}")

option(BUILD_DOC  "Build documentation (default ON)"   OFF)

#
# Enable testing for current directory and below
#
enable_testing()

set(GIT unknown)
if(EXISTS ${CMAKE_SOURCE_DIR}/../.git)
  find_package(Git)
  # Get the latest abbreviated commit hash of the working branch
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log --pretty=format:%h -n 1u
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH)
  set(GIT ${GIT_COMMIT_HASH})
  message(STATUS "Found Git repository, last commit hash: ${GIT}")
endif()

message(STATUS "${PROJECT_NAME} ${CMAKE_INSTALL_PREFIX}
    ${qucs-project_BINARY_DIR}" )

FIND_PACKAGE( Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED )

ADD_DEFINITIONS(${QT_DEFINITIONS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucs.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucs.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigi.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigi.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigilib.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigilib.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucsveri.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucsveri.1)

# Tools
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-activefilter/qucsactivefilter.1.cmake.in
  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-activefilter/qucsactivefilter.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs-filter/qucsfilter.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/qucs-filter/qucsfilter.1)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-attenuator/qucsattenuator.1.cmake.in
  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-attenuator/qucsattenuator.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs-lib/qucslib.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/qucs-lib/qucslib.1)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-powercombining/qucspowercombining.1.cmake.in
  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-powercombining/qucspowercombining.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs-rescodes/qucsrescodes.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/qucs-rescodes/qucsrescodes.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs-transcalc/qucstrans.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/qucs-transcalc/qucstrans.1)

IF (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-DQT_NO_DEBUG_OUTPUT)
ENDIF()

ADD_SUBDIRECTORY( man )
ADD_SUBDIRECTORY( qucs )
ADD_SUBDIRECTORY( qucs-core )
ADD_SUBDIRECTORY( qucs-activefilter )
ADD_SUBDIRECTORY( qucs-attenuator )
IF (BUILD_DOC)
  ADD_SUBDIRECTORY( qucs-doc )
ENDIF()
ADD_SUBDIRECTORY( qucs-filter )
ADD_SUBDIRECTORY( qucs-lib )
ADD_SUBDIRECTORY( qucs-transcalc )
ADD_SUBDIRECTORY( qucs-rescodes )
ADD_SUBDIRECTORY( examples )
ADD_SUBDIRECTORY( translations )
ADD_SUBDIRECTORY( contrib )
ADD_SUBDIRECTORY( qucs-powercombining )

#
# Run tests
#
add_test(NAME qucs_netlist_check
    COMMAND python3 run.py --qucs
    --prefix ${CMAKE_CURRENT_BINARY_DIR}/qucs/ --exclude skip.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/qucs-test/)
set_property(
    TEST qucs_netlist_check
    PROPERTY
        ENVIRONMENT "QUCS_LIBDIR=${CMAKE_CURRENT_SOURCE_DIR}/qucs-lib/library")
set_property(
    TEST qucs_netlist_check
    APPEND
    PROPERTY
        ENVIRONMENT "QUCSCONV=${CMAKE_CURRENT_BINARY_DIR}/qucs-core/src/converter/qucsconv")


add_test(NAME qucsator_check
    COMMAND python3 run.py --qucsator --exclude skip.txt
    --prefix ${CMAKE_CURRENT_BINARY_DIR}/qucs-core/src/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/qucs-test/)


add_test(NAME qucsator_equation_check
    COMMAND python3 run_equations.py
    --prefix ${CMAKE_CURRENT_BINARY_DIR}/qucs-core/src/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/qucs-test/)

INSTALL(DIRECTORY "examples" DESTINATION "share/${QUCS_NAME}")

#
# Custom uninstall target
#
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)

