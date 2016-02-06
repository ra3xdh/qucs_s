PROJECT(qucstrans CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
cmake_policy(VERSION 2.6)

# use top VERSION file
file (STRINGS ${PROJECT_SOURCE_DIR}/../VERSION QUCS_VERSION)
message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${QUCS_VERSION}")

set(PROJECT_VERSION "${QUCS_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2014")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

SET(CMAKE_BUILD_TYPE Debug)

ADD_DEFINITIONS( -DHAVE_CONFIG_H )

# define variables
SET(DOCDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/docs/")
SET(LANGUAGEDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/lang/")

# configure the header config.h
CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/../config.h.cmake"
    "${PROJECT_BINARY_DIR}/config.h"
)

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ") # enable warning level
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ") # enable C++11

# flag not available in mingw 4.8.2, MSVC10
IF(NOT WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register ")
ENDIF()

FIND_PACKAGE( Qt4 REQUIRED )
SET( QT_USE_QTGUI TRUE )

INCLUDE( ${QT_USE_FILE} )

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
  units.h
)

SET(QUCSTRANS_MOC_HDRS
  helpdialog.h
  optionsdialog.h
  qucstrans.h
)

QT4_WRAP_CPP( QUCSTRANS_MOC_SRCS ${QUCSTRANS_MOC_HDRS} )

SET( LIB_SRC
  c_microstrip.cpp
  coax.cpp
  coplanar.cpp
  microstrip.cpp
  rectwaveguide.cpp
  transline.cpp
)

SET(RESOURCES qucstrans.qrc)

QT4_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})

ADD_LIBRARY(transcalc ${LIB_SRC} )

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


ADD_EXECUTABLE(qucstrans MACOSX_BUNDLE WIN32
  ${QUCSTRANS_SRCS}
  ${QUCSTRANS_HDRS}
  ${QUCSTRANS_MOC_SRCS}
  ${RESOURCES_SRCS} )

TARGET_LINK_LIBRARIES( qucstrans ${QT_LIBRARIES} transcalc )

#INSTALL(TARGETS qucstrans DESTINATION bin)

#ADD_SUBDIRECTORY( bitmaps ) -> added as resources
ADD_SUBDIRECTORY( examples )

INSTALL( FILES qucstrans.1 DESTINATION share/man/man1 )

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

#
# Install the Qucs application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
#
INSTALL(TARGETS ${PROJECT_NAME}
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

