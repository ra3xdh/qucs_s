PROJECT(qucspowercombining CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
cmake_policy(VERSION 3.0)

SET(QUCS_NAME "qucs-s")

# use top VERSION file
file(STRINGS ${PROJECT_SOURCE_DIR}/../VERSION QUCS_VERSION)
message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${QUCS_VERSION}")

set(PROJECT_VERSION "${QUCS_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2014")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

SET(CMAKE_BUILD_TYPE Debug)

ADD_DEFINITIONS( -DHAVE_CONFIG_H )

# define variables
set(BINARYDIR "${CMAKE_INSTALL_PREFIX}/bin/")
set(BITMAPDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/bitmaps/")
set(DOCDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/docs/")
set(LANGUAGEDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/lang/")
set(LIBRARYDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/library/")
set(OCTAVEDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/octave/")

# configure the header config.h
CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/../config.h.cmake"
               "${PROJECT_BINARY_DIR}/config.h")

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ") # enable warning level
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ") # enable C++11

# flag not available in mingw 4.8.2, MSVC10
IF(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register ")
ENDIF()

FIND_PACKAGE( Qt5 COMPONENTS Core Gui Widgets Svg REQUIRED)
#INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES(
      ${Qt5Core_INCLUDE_DIRS}
      ${Qt5Widgets_INCLUDE_DIRS}
      ${Qt5Svg_INCLUDE_DIRS}
      )

# bug, the find package does not seem to set the QT_LIBRARIES, do it manually
SET(QT_LIBRARIES  ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Svg_LIBRARIES}  )

ADD_DEFINITIONS(${QT_DEFINITIONS})

set(QUCS-POWCOMB_SRCS qucspowercombiningtool.cpp main.cpp)

set(QUCS-POWCOMB_HDRS qucspowercombiningtool.h)

set(QUCS-POWCOMB_MOC_HDRS qucspowercombiningtool.h)

QT5_WRAP_CPP(QUCS-POWCOMB_MOC_SRCS ${QUCS-POWCOMB_MOC_HDRS})

SET(RESOURCES qucspowercombining.qrc)
QT5_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})

IF(APPLE)
  # set information on Info.plist file
  set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
  set(MACOSX_BUNDLE_GUI_IDENTIFIER
      "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
  set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
  set(MACOSX_BUNDLE_ICON_FILE qucspowercombining.icns)

  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucspowercombining.icns
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(QUCS-POWCOMB_SRCS
      ${QUCS-POWCOMB_SRCS}
      ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucspowercombining.icns)

ENDIF(APPLE)

ADD_EXECUTABLE(${QUCS_NAME}powercombining MACOSX_BUNDLE WIN32
  ${QUCS-POWCOMB_SRCS} ${QUCS-POWCOMB_HDRS} ${QUCS-POWCOMB_MOC_SRCS}
  ${RESOURCES_SRCS})

TARGET_LINK_LIBRARIES(${QUCS_NAME}powercombining ${QT_LIBRARIES})

# man pages
INSTALL(FILES qucspowercombining.1 DESTINATION share/man/man1)

#
# Prepare the installation
#
SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}")
IF(APPLE)
  SET(plugin_dest_dir ${PROJECT_NAME}.app/Contents/MacOS)
  SET(qtconf_dest_dir ${PROJECT_NAME}.app/Contents/Resources)
  SET(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app")
ENDIF(APPLE)

IF(WIN32)
  SET(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.exe")
ENDIF(WIN32)

INSTALL(TARGETS ${QUCS_NAME}powercombining
  BUNDLE DESTINATION bin COMPONENT Runtime
  RUNTIME DESTINATION bin COMPONENT Runtime
  )

#
# Install needed Qt plugins by copying directories from the qt installation
# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
#
IF(APPLE)
  INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION bin/${plugin_dest_dir}/plugins COMPONENT Runtime)
ENDIF()
#
# install a qt.conf file
# this inserts some cmake code into the install script to write the file
#
IF(APPLE)
INSTALL(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/bin/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime)
ENDIF()

#--------------------------------------------------------------------------------
# Use BundleUtilities to get all other dependencies for the application to work.
# It takes a bundle or executable along with possible plugins and inspects it
# for dependencies.  If they are not system dependencies, they are copied.

# directories to look for dependencies
IF(APPLE)
  SET(DIRS ${QT_LIBRARY_DIRS})
ENDIF()

# Now the work of copying dependencies into the bundle/package
# The quotes are escaped and variables to use at install time have their $ escaped
# An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
# Note that the image plugins depend on QtSvg and QtXml, and it got those copied
# over.
IF(APPLE)
INSTALL(CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/bin/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    " COMPONENT Runtime)
ENDIF()

