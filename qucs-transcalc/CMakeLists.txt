PROJECT(qucstrans CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 3.18)
cmake_policy(VERSION 3.18)

SET(QUCS_NAME "qucs-s")

# use top VERSION file
file (STRINGS ${PROJECT_SOURCE_DIR}/../VERSION QUCS_VERSION)
message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${QUCS_VERSION}")

set(PROJECT_VERSION "${QUCS_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2014")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")


add_compile_definitions(HAVE_CONFIG_H)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

# configure the header config.h
CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/../config.h.cmake"
    "${PROJECT_BINARY_DIR}/config.h"
)

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets )
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets )
include_directories(
      ${Qt${QT_VERSION_MAJOR}Core_INCLUDE_DIRS}
      ${Qt${QT_VERSION_MAJOR}Gui_INCLUDE_DIRS}
      ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS}
      )


IF(QT_VERSION_MAJOR EQUAL 6)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
ELSE()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
ENDIF()

if (MSVC)
  add_compile_options(/Zc:__cplusplus /permissive- /MP /Zc:preprocessor)
else()
  # additional warnings
  add_compile_options(-Wall -Wextra)
endif()

ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(QUCSTRANS_SRCS
  helpdialog.cpp
  main.cpp
  optionsdialog.cpp
  qucstrans.cpp
)

SET(QUCSTRANS_HDRS
  c_microstrip.h
  coax.h
  coplanar.h
  microstrip.h
  rectwaveguide.h
  transline.h
  stripline.h
  units.h
)

SET(QUCSTRANS_MOC_HDRS
  helpdialog.h
  optionsdialog.h
  qucstrans.h
)


SET( LIB_SRC
  c_microstrip.cpp
  coax.cpp
  coplanar.cpp
  microstrip.cpp
  rectwaveguide.cpp
  transline.cpp
  stripline.cpp
)

SET(RESOURCES qucstrans_.qrc)

IF(QT_VERSION_MAJOR EQUAL 6)
QT6_WRAP_CPP( QUCSTRANS_MOC_SRCS ${QUCSTRANS_MOC_HDRS} )
QT6_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})
ELSE()
QT5_WRAP_CPP( QUCSTRANS_MOC_SRCS ${QUCSTRANS_MOC_HDRS} )
QT5_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})
ENDIF()

ADD_LIBRARY(transcalc STATIC ${LIB_SRC} )

IF(APPLE)
  # set information on Info.plist file
	SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
  SET(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
  SET(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
  SET(MACOSX_BUNDLE_ICON_FILE qucstrans.icns)

  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucstrans.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(QUCSTRANS_SRCS ${QUCSTRANS_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucstrans.icns)

ENDIF(APPLE)


ADD_EXECUTABLE(${QUCS_NAME}trans MACOSX_BUNDLE WIN32
  ${QUCSTRANS_SRCS}
  ${QUCSTRANS_HDRS}
  ${QUCSTRANS_MOC_SRCS}
  ${RESOURCES_SRCS} )

TARGET_LINK_LIBRARIES( ${QUCS_NAME}trans Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets transcalc )
SET_TARGET_PROPERTIES(${QUCS_NAME}trans PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

#INSTALL(TARGETS ${QUCS_NAME}trans DESTINATION bin)

#ADD_SUBDIRECTORY( bitmaps ) -> added as resources
ADD_SUBDIRECTORY( examples )

#INSTALL( FILES qucstrans.1 DESTINATION share/man/man1 )

#
# Prepare the installation
#
SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(APPS "${CMAKE_INSTALL_PREFIX}/bin/${QUCS_NAME}trans")
IF(APPLE)
  SET(plugin_dest_dir ${QUCS_NAME}trans.app/Contents/MacOS)
  SET(qtconf_dest_dir ${QUCS_NAME}trans.app/Contents/Resources)
  SET(APPS "${CMAKE_INSTALL_PREFIX}/bin/${QUCS_NAME}trans.app")
ENDIF(APPLE)

IF(WIN32)
  SET(APPS "${CMAKE_INSTALL_PREFIX}/bin/${QUCS_NAME}trans.exe")
ENDIF(WIN32)

#
# Install the Qucs application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
#
INSTALL(TARGETS ${QUCS_NAME}trans
    BUNDLE DESTINATION bin COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
    )



#
# Install needed Qt plugins by copying directories from the qt installation
# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
#
IF(APPLE AND QT_PLUGINS_DIR)
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

