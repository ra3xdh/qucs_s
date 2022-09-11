PROJECT(qucsfilter CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
cmake_policy(VERSION 3.0)

SET(QUCS_NAME "qucs-s")

# use top VERSION file
file (STRINGS ${PROJECT_SOURCE_DIR}/../VERSION QUCS_VERSION)
message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${QUCS_VERSION}")

set(PROJECT_VERSION "${QUCS_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2014")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

#SET(CMAKE_BUILD_TYPE Debug)

ADD_DEFINITIONS( -DHAVE_CONFIG_H )


# configure the header config.h
CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/../config.h.cmake"
    "${PROJECT_BINARY_DIR}/config.h"
)

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ") # enable warning level
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ") # enable C++11


FIND_PACKAGE( Qt5 COMPONENTS Core Gui Widgets REQUIRED)

INCLUDE_DIRECTORIES(
      ${Qt5Core_INCLUDE_DIRS}
      ${Qt5Widgets_INCLUDE_DIRS}
      )


# bug, the find package does not seem to set the QT_LIBRARIES, do it manually
SET(QT_LIBRARIES  ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES}  )


ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(QUCS-FILTER_SRCS
  cline_filter.cpp
  eqn_filter.cpp
  filter.cpp
  helpdialog.cpp
  lc_filter.cpp
  line_filter.cpp
  main.cpp
  qf_cauer.cpp
  qf_filter.cpp
  qf_poly.cpp
  qucsfilter.cpp
  stepz_filter.cpp
  tl_filter.cpp
  quarterwave_filter.cpp
)

SET(QUCS-FILTER_HDRS
  cline_filter.h
  eqn_filter.h
  filter.h
  lc_filter.h
  line_filter.h
  material_props.h
  qf_cauer.h
  qf_filter.h
  qf_matrix.h
  qf_poly.h
  stepz_filter.h
  tl_filter.h
  quarterwave_filter.h
)

SET(QUCS-FILTER_MOC_HDRS
  helpdialog.h
  qucsfilter.h
)

QT5_WRAP_CPP(QUCS-FILTER_MOC_SRCS ${QUCS-FILTER_MOC_HDRS})

SET(RESOURCES qucsfilter.qrc)
QT5_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})


IF(APPLE)
  # set information on Info.plist file
	SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  SET(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
  SET(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
  SET(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
  SET(MACOSX_BUNDLE_ICON_FILE qucsfilter.icns)

  # set where in the bundle to put the icns file
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucsfilter.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  SET(QUCS-FILTER_SRCS ${QUCS-FILTER_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucsfilter.icns)

ENDIF(APPLE)

ADD_EXECUTABLE(${QUCS_NAME}filter MACOSX_BUNDLE WIN32
  ${QUCS-FILTER_SRCS}
  ${QUCS-FILTER_HDRS}
  ${QUCS-FILTER_MOC_SRCS}
  ${RESOURCES_SRCS} )

TARGET_LINK_LIBRARIES(${QUCS_NAME}filter ${QT_LIBRARIES})

# man pages
INSTALL(FILES qucsfilter.1 DESTINATION share/man/man1)

INSTALL(TARGETS ${QUCS_NAME}filter
    BUNDLE DESTINATION bin COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
    )

