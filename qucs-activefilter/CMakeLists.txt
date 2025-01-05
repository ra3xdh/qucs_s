PROJECT(qucsactivefilter CXX C)
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

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2014")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

#SET(CMAKE_BUILD_TYPE Debug)

add_compile_definitions(HAVE_CONFIG_H)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

#configure the header config.h
CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/../config.h.cmake"
    "${PROJECT_BINARY_DIR}/config.h"
)

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

  find_package(Qt6 REQUIRED COMPONENTS Core  Widgets Svg SvgWidgets)
  include_directories(
      ${Qt6Core_INCLUDE_DIRS}
      ${Qt6Widgets_INCLUDE_DIRS}
      ${Qt6Svg_INCLUDE_DIRS}
      ${Qt6SvgWidgets_INCLUDE_DIRS}
      )

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

ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(QUCS-ACTIVE-FILTER_SRCS
filter.cpp
mfbfilter.cpp
main.cpp
qf_poly.cpp
sallenkey.cpp
schcauer.cpp
transferfuncdialog.cpp
helpdialog.cpp
qucsactivefilter.cpp
)

SET(QUCS-ACTIVE-FILTER_MOC_HDRS
transferfuncdialog.h
qucsactivefilter.h
helpdialog.h
)

SET(RESOURCES qucsactivefilter.qrc)
QT6_WRAP_CPP(QUCS-ACTIVE-FILTER_MOC_SRCS ${QUCS-ACTIVE-FILTER_MOC_HDRS})
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
  SET(MACOSX_BUNDLE_ICON_FILE qucsactivefilter.icns)

  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucsactivefilter.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(QUCS-ACTIVE-FILTER_SRCS ${QUCS-ACTIVE-FILTER_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucsactivefilter.icns)

ENDIF(APPLE)

ADD_EXECUTABLE(${QUCS_NAME}activefilter MACOSX_BUNDLE WIN32
  ${QUCS-ACTIVE-FILTER_SRCS}
  ${QUCS-ACTIVE-FILTER_MOC_SRCS}
  ${RESOURCES_SRCS} )


TARGET_LINK_LIBRARIES(${QUCS_NAME}activefilter Qt6::Core Qt6::Widgets Qt6::Svg Qt6::SvgWidgets)


SET_TARGET_PROPERTIES(${QUCS_NAME}activefilter PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

INSTALL(TARGETS ${QUCS_NAME}activefilter
    BUNDLE DESTINATION bin COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
    )

