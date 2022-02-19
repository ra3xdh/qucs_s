PROJECT(qucs-suite CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
cmake_policy(VERSION 2.8)

SET(QUCS_NAME "qucs-s")

file (STRINGS "${qucs-suite_SOURCE_DIR}/VERSION" QUCS_VERSION)
message(STATUS "Configuring Qucs: VERSION ${QUCS_VERSION}")

set(GIT unknown)
IF(EXISTS ${CMAKE_SOURCE_DIR}/../.git )
  FIND_PACKAGE(Git)
  # Get the latest abbreviated commit hash of the working branch
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log --pretty=format:%h -n 1u
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
  )
  set(GIT ${GIT_COMMIT_HASH})
  message(STATUS "Found Git repository, last commit hash: ${GIT}")
ENDIF()

message(STATUS "${PROJECT_NAME} ${CMAKE_INSTALL_PREFIX}
    ${qucs-suite_BINARY_DIR}" )

FIND_PACKAGE( Qt5 COMPONENTS Core Gui Widgets LinguistTools Script REQUIRED )

ADD_DEFINITIONS(${QT_DEFINITIONS})

IF (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-DQT_NO_DEBUG_OUTPUT)
ENDIF()

ADD_SUBDIRECTORY( qucs )
ADD_SUBDIRECTORY( qucs-activefilter )
#ADD_SUBDIRECTORY( qucs-doc )
ADD_SUBDIRECTORY( qucs-filter )
ADD_SUBDIRECTORY( qucs-lib )
#ADD_SUBDIRECTORY( examples )
ADD_SUBDIRECTORY( translations )

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


