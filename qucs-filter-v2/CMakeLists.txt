PROJECT(qucsfilter-v2 CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
cmake_policy(VERSION 2.6)

SET(DOCDIR TRUE)
SET(LANGUAGEDIR TRUE)

SET(CMAKE_BUILD_TYPE Debug)

# The incomplete filter-v2 only builds withs -stdlib=libstc++
# OSX defaults to libstc++. \fixme
IF(APPLE)
  SET(CMAKE_CXX_FLAGS  "-stdlib=libstdc++")
ENDIF()


ADD_DEFINITIONS( -DHAVE_CONFIG_H )

# configure the header config.h
CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/../config.h.cmake"
    "${PROJECT_BINARY_DIR}/config.h"
)

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

FIND_PACKAGE( Qt4 REQUIRED )
SET( QT_USE_QTGUI TRUE )
SET( QT_USE_QTXML TRUE )
SET( QT_USE_QT3SUPPORT TRUE )

INCLUDE( ${QT_USE_FILE} )

ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(QUCS-FILTER2_SRCS
qf_api.cpp
qf_bessel.cpp
qf_blinch.cpp
qf_box.cpp
qf_butcheb.cpp
qf_cauer.cpp
qf_common.cpp
qf_comp.cpp
qf_dialog.cpp
qf_filter.cpp
qf_icheb.cpp
qf_legendre.cpp
qf_main.cpp
qf_matrix.cpp
qf_optimize.cpp
qf_poly.cpp
qf_qr.cpp
qf_settings.cpp
qf_tform.cpp
#qf_transform.cpp
qf_zigzag.cpp
)

SET(QUCS-FILTER2_MOC_HDRS
qf_box.h
qf_dialog.h
)

QT4_WRAP_CPP(QUCS-FILTER2_MOC_SRCS ${QUCS-FILTER2_MOC_HDRS})

ADD_EXECUTABLE(qucs-filter-v2 ${QUCS-FILTER2_SRCS} ${QUCS-FILTER2_MOC_SRCS})

TARGET_LINK_LIBRARIES(qucs-filter-v2 ${QT_LIBRARIES})

INSTALL(TARGETS qucs-filter-v2 DESTINATION bin)


