PROJECT(qucs-suite CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
cmake_policy(VERSION 2.8)


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

# Build qucs-core (release mode) FIXME
# * during release the repo directories are flattened
# * simply adding qucs-core, creates target name collision qucs vs (lib)qucs.
#  * targets must be globally unique
# * external_project, does a copy (!)
#  * this is perfect for 'external' tarbals or repos, but we have the code
#    already in the tree, can we resolve the collision in another way?
include(ExternalProject)
IF(EXISTS ${CMAKE_SOURCE_DIR}/qucs-core )
  ADD_SUBDIRECTORY( qucs-core )
  #ExternalProject_Add ( qucs-core
  #  URL ${qucs-suite_SOURCE_DIR}/qucs-core/
  #  PREFIX  ${qucs-suite_BINARY_DIR}
  #  INSTALL_DIR ${CMAKE_INSTALL_DIR}
  #  INSTALL_COMMAND "make install"
  #  #CMAKE_ARGS ${qucs-suite_CMAKE_ARGS}
  #  )
ENDIF()

FIND_PACKAGE( Qt4 REQUIRED )

SET( QT_USE_QT TRUE )
#SET( QT_USE_QTXML TRUE )
#SET( QT_USE_QTSVG TRUE )
SET( QT_USE_QTCORE TRUE )
SET( QT_USE_QTGUI TRUE )

SET( QT_USE_QTSCRIPT TRUE )

SET( QT_DEBUG TRUE)

INCLUDE( ${QT_USE_FILE} )

ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/qucs
        #${CMAKE_SOURCE_DIR}/bitmaps ->no sources here
        ${CMAKE_SOURCE_DIR}/components
        ${CMAKE_SOURCE_DIR}/diagrams
        ${CMAKE_SOURCE_DIR}/dialogs
        #${CMAKE_SOURCE_DIR}/octave ->no sources here
        ${CMAKE_SOURCE_DIR}/paintings
        )

ADD_SUBDIRECTORY( qucs )
ADD_SUBDIRECTORY( qucs-activefilter )
ADD_SUBDIRECTORY( qucs-attenuator )
#ADD_SUBDIRECTORY( qucs-doc )
ADD_SUBDIRECTORY( qucs-edit )
ADD_SUBDIRECTORY( qucs-filter )
ADD_SUBDIRECTORY( qucs-help )
ADD_SUBDIRECTORY( qucs-lib )
ADD_SUBDIRECTORY( qucs-transcalc )
ADD_SUBDIRECTORY( qucs-rescodes )
#ADD_SUBDIRECTORY( examples )
ADD_SUBDIRECTORY( translations )
ADD_SUBDIRECTORY( contrib )


#
# Custom uninstall target
#
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)


