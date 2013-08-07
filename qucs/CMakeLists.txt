# To have CMake pick a Qt installation of your choice that won't be found
# automatically, set the CMAKE_PREFIX_PATH environment variable.
# For example: "export CMAKE_PREFIX_PATH=/usr/local/trolltech/qt4.3.5"

PROJECT(qucs CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
cmake_policy(VERSION 2.6)

set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "0")
set(PROJECT_VERSION_PATCH "18")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2013")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

#
SET(CMAKE_BUILD_TYPE Debug)

if(UNIX AND NOT APPLE)
  #string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)
  #set(BIN_INSTALL_DIR "bin")
  #set(DOC_INSTALL_DIR "share/doc/${PROJECT_NAME_LOWERCASE}/")
else()
  set(BIN_INSTALL_DIR ".")
  set(DOC_INSTALL_DIR ".")
endif()

#
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

# define variables
SET(BINARYDIR "${CMAKE_INSTALL_PREFIX}/bin/")
#SET(BITMAPDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/bitmaps/")
SET(DOCDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/docs/")
SET(LANGUAGEDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/lang/")
SET(LIBRARYDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/library/")
SET(OCTAVEDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/octave/")

# configure the header config.h
CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/../config.h.cmake"
    "${PROJECT_BINARY_DIR}/config.h"
)

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} )

FIND_PACKAGE( Qt4 REQUIRED )

SET( QT_USE_QT TRUE )
SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTSVG TRUE )
SET( QT_USE_QTCORE TRUE )
SET( QT_USE_QTGUI TRUE )
SET( QT_USE_QT3SUPPORT TRUE )

#INCLUDE( ${QT_USE_FILE} )

ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE( ${QT_USE_FILE} )

#ADD_SUBDIRECTORY( bitmaps ) -> added as resources
ADD_SUBDIRECTORY( components )
ADD_SUBDIRECTORY( diagrams )
ADD_SUBDIRECTORY( dialogs )
ADD_SUBDIRECTORY( octave )
ADD_SUBDIRECTORY( paintings )

SET(QUCS_SRCS
element.cpp	octave_window.cpp	qucsdoc.cpp
textdoc.cpp  main.cpp	schematic.cpp
mnemo.cpp	qucs.cpp	viewpainter.cpp
module.cpp	schematic_element.cpp	wire.cpp
mouseactions.cpp qucs_actions.cpp	schematic_file.cpp
wirelabel.cpp node.cpp qucs_init.cpp
syntax.cpp
)

SET(QUCS_MOC_HDRS
octave_window.h
qucs.h
schematic.h
textdoc.h
)

# headers that need to be moc'ed
QT4_WRAP_CPP( QUCS_MOC_SRCS ${QUCS_MOC_HDRS} )

# generate rules for building source files from the resources
SET(RESOURCES qucs.qrc)
QT4_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})

# configure Apple bundle information 
IF(APPLE)
  # set information on Info.plist file
	SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
  SET(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
  SET(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
  SET(MACOSX_BUNDLE_ICON_FILE qucs.icns) 

  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/bitmaps/qucs.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(QUCS_SRCS ${QUCS_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/bitmaps/qucs.icns)
ENDIF(APPLE)

#
#  CMake's way of creating an executable
#
ADD_EXECUTABLE( qucs MACOSX_BUNDLE WIN32
  ${QUCS_SRCS}
  ${QUCS_MOC_SRCS}
  ${RESOURCES_SRCS} )

#
# Tell CMake which libraries we need to link our executable against.
#
TARGET_LINK_LIBRARIES( qucs ${QT_LIBRARIES} components diagrams dialogs paintings )

#
# Now the installation stuff below
#
SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}")
IF(APPLE)
  SET(plugin_dest_dir ${PROJECT_NAME}.app/Contents/MacOS)
  SET(qtconf_dest_dir ${PROJECT_NAME}.app/Contents/Resources)
  SET(APPS "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app")
ENDIF(APPLE)
IF(WIN32)
  SET(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.exe")
ENDIF(WIN32)

#
# Install the Qucs application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
#
INSTALL(TARGETS qucs 
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
    )

#
# Install needed Qt plugins by copying directories from the qt installation
# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
#
INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)

#
# install a qt.conf file
# this inserts some cmake code into the install script to write the file
#
INSTALL(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime)


#--------------------------------------------------------------------------------
# Use BundleUtilities to get all other dependencies for the application to work.
# It takes a bundle or executable along with possible plugins and inspects it
# for dependencies.  If they are not system dependencies, they are copied.

# directories to look for dependencies
SET(DIRS ${QT_LIBRARY_DIRS})

# Now the work of copying dependencies into the bundle/package
# The quotes are escaped and variables to use at install time have their $ escaped
# An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
# Note that the image plugins depend on QtSvg and QtXml, and it got those copied
# over.
INSTALL(CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    " COMPONENT Runtime)


# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# where CPackConfig.cmake is created by including CPack
# And then there's ways to customize this as well
set(CPACK_BINARY_DRAGNDROP ON)
include(CPack)




#--------------------------------------------------------------------------------
# TODO rule to update Qucs strings
# TODO add translatins as resource
#SET(TRANSLATIONS qucs_de.ts qucs_pl.ts qucs_fr.ts qucs_it.ts qucs_ro.ts
#  qucs_pt.ts qucs_es.ts qucs_jp.ts qucs_hu.ts qucs_he.ts qucs_sv.ts
#  qucs_tr.ts qucs_uk.ts qucs_ru.ts qucs_cs.ts qucs_ca.ts qucs_ar.ts
#  qucs_kk.ts)

# TODO Include Qt strings
#set(GENTRANSLATIONS qtgeneric_untranslated.ts qtgeneric_de.ts qtgeneric_fr.ts
#  qtgeneric_es.ts qtgeneric_ro.ts qtgeneric_jp.ts qtgeneric_hu.ts
#  qtgeneric_it.ts qtgeneric_he.ts qtgeneric_sv.ts qtgeneric_pl.ts
#  qtgeneric_tr.ts qtgeneric_uk.ts qtgeneric_ru.ts qtgeneric_cs.ts
#  qtgeneric_ca.ts qtgeneric_ar.ts qtgeneric_kk.ts)

#QT4_ADD_TRANSLATION(LANG_SRCS ${LAN_FILES})
#INSTALL( FILES ${LANG_SRCS} DESTINATION share/qucs/lang/)


