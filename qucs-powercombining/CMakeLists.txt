PROJECT(qucspowercombining CXX C)
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
set(PROJECT_COPYRIGHT_YEAR "2016")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

#SET(CMAKE_BUILD_TYPE Debug)

add_compile_definitions(HAVE_CONFIG_H)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

# configure the header config.h
CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/../config.h.cmake"
    "${PROJECT_BINARY_DIR}/config.h"
)

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

if(WITH_QT6)
    set(QT_VERSION_MAJOR 6)
else()
    set(QT_VERSION_MAJOR 5)
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core  Widgets Svg SvgWidgets)
    include_directories(
        ${Qt${QT_VERSION_MAJOR}Core_INCLUDE_DIRS}
        ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS}
        ${Qt${QT_VERSION_MAJOR}Svg_INCLUDE_DIRS}
        ${Qt${QT_VERSION_MAJOR}SvgWidgets_INCLUDE_DIRS}
        )
else()
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core  Widgets Svg)
    include_directories(
        ${Qt${QT_VERSION_MAJOR}Core_INCLUDE_DIRS}
        ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS}
        ${Qt${QT_VERSION_MAJOR}Svg_INCLUDE_DIRS}
        )
endif()

set(QT_VERSION ${Qt${QT_VERSION_MAJOR}Core_VERSION})

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

SET(QUCS-POWCOMB_SRCS
  qucspowercombiningtool.cpp
  main.cpp
)

SET(QUCS-POWCOMB_HDRS
  qucspowercombiningtool.h
)

SET(QUCS-POWCOMB_MOC_HDRS
  qucspowercombiningtool.h
)

SET(RESOURCES qucspowercombining.qrc)
IF(QT_VERSION_MAJOR EQUAL 6)
QT6_WRAP_CPP(QUCS-POWCOMB_MOC_SRCS ${QUCS-POWCOMB_MOC_HDRS})
QT6_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})
ELSE()
QT5_WRAP_CPP(QUCS-POWCOMB_MOC_SRCS ${QUCS-POWCOMB_MOC_HDRS})
QT5_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})
ENDIF()

IF(APPLE)
  # set information on Info.plist file
	SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
  SET(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
  SET(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
  SET(MACOSX_BUNDLE_ICON_FILE qucspowercombining.icns)

  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucspowercombining.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(QUCS-POWCOMB_SRCS ${QUCS-POWCOMB_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucspowercombining.icns)

ENDIF(APPLE)

ADD_EXECUTABLE(${QUCS_NAME}powercombining MACOSX_BUNDLE WIN32
  ${QUCS-POWCOMB_SRCS}
  ${QUCS-POWCOMB_HDRS}
  ${QUCS-POWCOMB_MOC_SRCS}
  ${RESOURCES_SRCS} )

if(QT_VERSION_MAJOR EQUAL 6)
    TARGET_LINK_LIBRARIES(${QUCS_NAME}powercombining Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Svg Qt${QT_VERSION_MAJOR}::SvgWidgets)
else()
    TARGET_LINK_LIBRARIES(${QUCS_NAME}powercombining Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Svg )
endif()

SET_TARGET_PROPERTIES(${QUCS_NAME}powercombining PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

INSTALL(TARGETS ${QUCS_NAME}powercombining
    BUNDLE DESTINATION bin COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
    )

