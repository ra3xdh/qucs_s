# To have CMake pick a Qt installation of your choice that won't be found
# automatically, set the CMAKE_PREFIX_PATH environment variable.
# For example: "export CMAKE_PREFIX_PATH=/usr/local/trolltech/qt4.3.5"
#
# You can change the install location by
# running cmake like this:
#   mkdir build; cd build
#   cmake .. -DCMAKE_INSTALL_PREFIX=/new/install/prefix
#
# By default, the prefix is "/usr/local"

PROJECT(qucs-s CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
cmake_policy(VERSION 3.10)

SET(QUCS_NAME "${PROJECT_NAME}")

# use top VERSION file
file (STRINGS ${PROJECT_SOURCE_DIR}/../VERSION QUCS_VERSION)

if(DEFINED CI_VERSION)
    set(PROJECT_VERSION "${CI_VERSION}")
else()
    set(PROJECT_VERSION "${QUCS_VERSION}")
endif()

message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${PROJECT_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2014")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

# If Git hash not defined, try to define it
#IF(NOT GIT)
#  IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../.git )
#    FIND_PACKAGE(Git)
#    # Get the latest abbreviated commit hash of the working branch
#    execute_process(
#      COMMAND ${GIT_EXECUTABLE} log --pretty=format:%h -n 1u
#      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#      OUTPUT_VARIABLE GIT_COMMIT_HASH
#    )
#    set(GIT ${GIT_COMMIT_HASH})
#    message(STATUS "Found Git repository, last commit hash: ${GIT}")
#  ENDIF()
#ENDIF()

if(UNIX AND NOT APPLE)
  #string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)
  #set(BIN_INSTALL_DIR "bin")
  #set(DOC_INSTALL_DIR "share/doc/${PROJECT_NAME_LOWERCASE}/")
else()
  #set(BIN_INSTALL_DIR ".")
  #set(DOC_INSTALL_DIR ".")
endif()

#
add_compile_definitions(HAVE_CONFIG_H)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg Xml PrintSupport)
find_package(muparser REQUIRED) # Find muParser
include_directories(
  ${Qt6Core_INCLUDE_DIRS}
  ${Qt6Widgets_INCLUDE_DIRS}
  ${Qt6Svg_INCLUDE_DIRS}
  ${Qt6Xml_INCLUDE_DIRS}
  ${Qt6PrintSupport_INCLUDE_DIRS}
  ${MUPARSER_INCLUDE_DIRS}
)



#MESSAGE("QT_INCLUDES=[${QT_INCLUDES}]")
#MESSAGE("QT_LIBRARIES=[${QT_LIBRARIES}]")
# configure the header config.h
CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/../config.h.cmake"
    "${PROJECT_BINARY_DIR}/config.h"
)

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_VERSION ${Qt6Core_VERSION})

if (${QT_VERSION} VERSION_LESS "6.7.0")
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_definitions(_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
        add_compile_options(/permissive- /Zc:__cplusplus /Zc:preprocessor /MP /Od /vmg)
        add_compile_options(/wd4244 /wd4267 /wd4312)
    else()
        add_compile_options(-Wall -Wextra -O0 -g)
        if (CMAKE_CXX_COMPILER_ID MATCHES "^AppleClang$|^Clang$")
            add_compile_options(-Wno-ignored-attributes)
        endif()
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_definitions(_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
        string(REGEX REPLACE "/W1" "/w" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        add_compile_options(/permissive- /Zc:__cplusplus /Zc:preprocessor /MP /vmg)
        add_link_options(/OPT:REF /OPT:ICF)
    else()
        add_compile_options(-w)
    endif()
endif()

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  #${qucs_SOURCE_DIR}/bitmaps ->no sources here
  ${PROJECT_SOURCE_DIR}/components
  ${PROJECT_SOURCE_DIR}/diagrams
  ${PROJECT_SOURCE_DIR}/dialogs
  #${qucs_SOURCE_DIR}/octave ->no sources here
  ${PROJECT_SOURCE_DIR}/paintings
  ${PROJECT_SOURCE_DIR}/third_party
  )

#ADD_SUBDIRECTORY( bitmaps ) -> added as resources
ADD_SUBDIRECTORY( qt3_compat )
ADD_SUBDIRECTORY( components )
ADD_SUBDIRECTORY( diagrams )
ADD_SUBDIRECTORY( dialogs )
ADD_SUBDIRECTORY( octave )
ADD_SUBDIRECTORY( python )
ADD_SUBDIRECTORY( paintings )
ADD_SUBDIRECTORY( extsimkernels )
ADD_SUBDIRECTORY( spicecomponents )

SET(QUCS_SRCS
  element.cpp	octave_window.cpp	qucsdoc.cpp
  textdoc.cpp  main.cpp	schematic.cpp
  mnemo.cpp	qucs.cpp
  module.cpp	schematic_element.cpp	wire.cpp schematic_render.cpp
  mouseactions.cpp qucs_actions.cpp	schematic_file.cpp
  wirelabel.cpp node.cpp qucs_init.cpp
  syntax.cpp misc.cpp messagedock.cpp
  settings.cpp
  imagewriter.cpp printerwriter.cpp projectView.cpp
  symbolwidget.cpp wire_planner.cpp
)

SET(QUCS_HDRS
element.h
conductor.h
main.h
messagedock.h
misc.h
mnemo.h
module.h
mouseactions.h
node.h
octave_window.h
qucs.h
qucsdoc.h
schematic.h
settings.h
syntax.h
symbolwidget.h
textdoc.h
wire.h
wirelabel.h
wire_planner.h
)

#
# files that have Q_OBJECT need to be MOC'ed
#
SET(QUCS_MOC_HDRS
  octave_window.h
  qucs.h
  schematic.h
  textdoc.h
  messagedock.h
  projectView.h
  symbolwidget.h
)

# headers that need to be moc'ed
# generate rules for building source files from bitmap resources
SET(RESOURCES qucs.qrc)
Qt6_WRAP_CPP( QUCS_MOC_SRCS ${QUCS_MOC_HDRS} )
Qt6_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})

if(UNIX AND NOT APPLE)

    SET(ICON16 bitmaps/hicolor/16x16/apps/${QUCS_NAME}.png )
    SET(ICON22 bitmaps/hicolor/22x22/apps/${QUCS_NAME}.png )
    SET(ICON32 bitmaps/hicolor/32x32/apps/${QUCS_NAME}.png )
    SET(ICON48 bitmaps/hicolor/48x48/apps/${QUCS_NAME}.png )
    SET(ICON64 bitmaps/hicolor/64x64/apps/${QUCS_NAME}.png )
    SET(ICON128 bitmaps/hicolor/128x128/apps/${QUCS_NAME}.png )
    SET(ICON256 bitmaps/hicolor/256x256/apps/${QUCS_NAME}.png )
    SET(ICON512 bitmaps/hicolor/512x512/apps/${QUCS_NAME}.png )
    SET(ICONsc bitmaps/hicolor/scalable/apps/${QUCS_NAME}.svg )

    INSTALL( FILES ${ICON16} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/16x16/apps)
    INSTALL( FILES ${ICON22} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/22x22/apps)
    INSTALL( FILES ${ICON32} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/32x32/apps)
    INSTALL( FILES ${ICON48} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps)
    INSTALL( FILES ${ICON64} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/64x64/apps)
    INSTALL( FILES ${ICON128} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/128x128/apps)
    INSTALL( FILES ${ICON256} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/256x256/apps)
    INSTALL( FILES ${ICON512} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/512x512/apps)
    INSTALL( FILES ${ICONsc} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)

    SET(DESKTOP ${QUCS_NAME}.desktop )
    INSTALL( FILES ${DESKTOP} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
endif()




#
# configure Apple bundle information
#
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

    # This tells cmake where to place the translations inside the bundle
    #SET_SOURCE_FILES_PROPERTIES( ${LANG_SRCS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/lang )
    # include the translation files in the target
    #SET(RESOURCES_SRCS ${RESOURCES_SRCS} ${LANG_SRCS})
ENDIF(APPLE)

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/qucs_icon.rc")
#
#  CMake's way of creating an executable
#
ADD_EXECUTABLE( ${QUCS_NAME} MACOSX_BUNDLE WIN32
  ${QUCS_HDRS}
  ${QUCS_SRCS}
  ${QUCS_MOC_SRCS}
  ${RESOURCES_SRCS}
  ${app_icon_resource_windows}
 )

#
# Tell CMake which libraries we need to link our executable against.
#
TARGET_LINK_LIBRARIES( ${QUCS_NAME}
    components diagrams dialogs paintings extsimkernels spicecomponents qt3_compat
    Qt6::Core  Qt6::Gui  Qt6::Widgets Qt6::Svg  Qt6::Xml  Qt6::PrintSupport
    muparser
)

SET_TARGET_PROPERTIES(${QUCS_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
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
# Install the Qucs application, on Apple, the bundle is
# installed as on other platforms it'll go into the bin directory.
#
INSTALL(TARGETS ${QUCS_NAME}
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

INSTALL( FILES ${SCRIPTS} DESTINATION bin/ )

INSTALL( FILES ${QUCS_NAME}.1 DESTINATION share/man/man1 )

# Install wrapper scripts
IF(WIN32)
  SET(SCRIPTS qucs_run_hdl.bat qucs_run_verilog.bat qucs_mkdigilib.bat)
ELSE()
  SET(SCRIPTS qucs_run_hdl qucs_run_verilog qucs_mkdigilib)
ENDIF()
INSTALL( FILES ${SCRIPTS} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ DESTINATION bin/ )


# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# where CPackConfig.cmake is created by including CPack
# And then there's ways to customize this as well
#set(CPACK_BINARY_DRAGNDROP ON)
#include(CPack)



