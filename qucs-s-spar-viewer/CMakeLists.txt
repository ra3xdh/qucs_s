PROJECT(qucs-s-spar-viewer CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
cmake_policy(VERSION 3.10)

SET(QUCS_NAME "qucs-s")

# use top VERSION file
file (STRINGS ${PROJECT_SOURCE_DIR}/../VERSION QUCS_VERSION)

if(DEFINED CI_VERSION)
    set(PROJECT_VERSION "${CI_VERSION}")
else()
    set(PROJECT_VERSION "${QUCS_VERSION}")
endif()

message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${PROJECT_VERSION}")

set(PROJECT_VENDOR "Qucs-S team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2024")
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

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)
include_directories(
      ${Qt6Core_INCLUDE_DIRS}
      ${Qt6Gui_INCLUDE_DIRS}
      ${Qt6Widgets_INCLUDE_DIRS}
      )


find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
include_directories(
      ${Qt6Core_INCLUDE_DIRS}
      ${Qt6Gui_INCLUDE_DIRS}
      ${Qt6Widgets_INCLUDE_DIRS}
      )

if(Qt6_FOUND)
    set(QT_VERSION ${Qt6Core_VERSION})
endif()

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
        add_compile_options(/fsanitize=address)
        add_link_options(/fsanitize=address)
    else()
        add_compile_options(-Wall -Wextra -O0 -g)
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
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

ADD_DEFINITIONS(${QT_DEFINITIONS})

file(GLOB PLOT_WIDGETS_SOURCES
     PlotWidgets/*.cpp
)

file(GLOB PLOT_WIDGETS_HEADERS
     PlotWidgets/*.h
)

file(GLOB QCUSTOMPLOT_SOURCE
     PlotWidgets/QCustomPlot/qcustomplot.cpp
)

file(GLOB QCUSTOMPLOT_HEADER
     PlotWidgets/QCustomPlot/qcustomplot.h
)

file(GLOB CUSTOM_WIDGETS_SOURCES
     CustomWidgets/*.cpp
)

file(GLOB CUSTOM_WIDGETS_HEADERS
     CustomWidgets/*.h
)

file(GLOB SCHEMATIC_SOURCES
     Schematic/*.cpp
     Schematic/Export/*.cpp
     Schematic/Export/QucsS/*.cpp
     Schematic/Export/QucsS/Microstrip/*.cpp
     Schematic/PaintingFunctions/*.cpp
     Schematic/PaintingFunctions/Microstrip/*.cpp
     Schematic/PaintingFunctions/IdealTransmissionLines/*.cpp
)

file(GLOB SCHEMATIC_HEADERS
     Schematic/*.h
)

file(GLOB MISC_SOURCES
     Misc/*.cpp
)

file(GLOB MISC_HEADERS
     Misc/*.h
)

file(GLOB SPAR_SOURCES
     SPAR/*.cpp
)

file(GLOB SPAR_HEADERS
     SPAR/*.h
)

file(GLOB SPAR_SPECIAL_COMPONENTS
    SPAR/SpecialComponents/*.cpp
    SPAR/SpecialComponents/MicrostripComponents/*.cpp
)


file(GLOB UI_SOURCES
     UI/*.cpp
)

file(GLOB UI_HEADERS
     UI/*.h
)

## TOOLS
file(GLOB TOOLS_SOURCES
     Tools/Filtering/*.cpp
     Tools/Filtering/DirectCoupledFilters/*.cpp
     Tools/PowerCombining/*.cpp
     Tools/AttenuatorDesign/*.cpp
     Tools/NetlistScratchPad/*.cpp
     Tools/SimulationSetup/*.cpp
     Tools/MatchingNetwork/*.cpp
     Tools/TransmissionLineSynthesis/*.cpp
)

file(GLOB TOOLS_HEADERS
     Tools/Filtering/*.h
     Tools/Filtering/DirectCoupledFilters/*.h
     Tools/PowerCombining/*.h
     Tools/AttenuatorDesign/*.h
     Tools/NetlistScratchPad/*.h
     Tools/SimulationSetup/*.h
     Tools/MatchingNetwork/*.h
     Tools/TransmissionLineSynthesis/*.h
)


set(spar_viewer_sources
    main.cpp
    ${UI_SOURCES}
    ${PLOT_WIDGETS_SOURCES}
    ${CUSTOM_WIDGETS_SOURCES}
    ${SCHEMATIC_SOURCES}
    ${SPAR_SOURCES}
    ${SPAR_SPECIAL_COMPONENTS}
    ${MISC_SOURCES}
    ${TOOLS_SOURCES}
    ${QCUSTOMPLOT_SOURCE}
)

set(spar_viewer_moc_headers
    ${UI_HEADERS}
    ${PLOT_WIDGETS_HEADERS}
    ${CUSTOM_WIDGETS_HEADERS}
    ${SCHEMATIC_HEADERS}
    ${SPAR_HEADERS}
    ${MISC_HEADERS}
    ${TOOLS_HEADERS}
    ${QCUSTOMPLOT_HEADER}
)

SET(RESOURCES qucs-s-spar-viewer.qrc)

QT6_WRAP_CPP( spar_viewer_moc_sources ${spar_viewer_moc_headers} )
QT6_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})

IF(APPLE)
  # set information on Info.plist file
	SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
  SET(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
  SET(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
  SET(MACOSX_BUNDLE_ICON_FILE qucs-s-spar-viewer.icns)

  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucs-s-spar-viewer.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(spar_viewer_sources ${spar_viewer_sources} ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucs-s-spar-viewer.icns)

ENDIF(APPLE)

ADD_EXECUTABLE( ${QUCS_NAME}spar-viewer MACOSX_BUNDLE WIN32
  ${spar_viewer_sources}
  ${spar_viewer_moc_sources}
  ${RESOURCES_SRCS}
  Misc/readTouchstone.cpp
)

TARGET_LINK_LIBRARIES( ${QUCS_NAME}spar-viewer Qt6::Core Qt6::Gui Qt6::Widgets Qt6::PrintSupport)
SET_TARGET_PROPERTIES(${QUCS_NAME}spar-viewer PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
#INSTALL (TARGETS ${QUCS_NAME}spar-viewer DESTINATION bin)
#
# Prepare the installation
#
SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(APPS "${CMAKE_INSTALL_PREFIX}/bin/${QUCS_NAME}spar-viewer")
IF(APPLE)
  SET(plugin_dest_dir ${QUCS_NAME}spar-viewer.app/Contents/MacOS)
  SET(qtconf_dest_dir ${QUCS_NAME}spar-viewer.app/Contents/Resources)
  SET(APPS "${CMAKE_INSTALL_PREFIX}/bin/${QUCS_NAME}spar-viewer.app")
ENDIF(APPLE)

IF(WIN32)
  SET(APPS "${CMAKE_INSTALL_PREFIX}/bin/${QUCS_NAME}spar-viewer.exe")
ENDIF(WIN32)

#
# Install the Qucs application, on Apple, the bundle is
# installed as on other platforms it'll go into the bin directory.
#
INSTALL(TARGETS ${QUCS_NAME}spar-viewer
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


