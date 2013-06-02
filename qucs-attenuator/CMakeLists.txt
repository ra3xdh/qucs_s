PROJECT(qucsattenuator CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
cmake_policy(VERSION 2.6)

SET(BITMAPDIR TRUE)
SET(BINARYDIR TRUE)
SET(DOCDIR TRUE)
SET(LANGUAGEDIR TRUE)
SET(LIBRARYDIR TRUE)

SET(CMAKE_BUILD_TYPE Debug)

ADD_DEFINITIONS( -DHAVE_CONFIG_H )

# configure the header config.h
CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/../config.h.cmake"
    "${PROJECT_BINARY_DIR}/config.h"
)

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

FIND_PACKAGE( Qt4 REQUIRED )
SET( QT_USE_QTGUI TRUE )
#SET( QT_USE_QT3SUPPORT TRUE )

INCLUDE( ${QT_USE_FILE} )

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_SUBDIRECTORY( bitmaps )

SET( attenuator_sources attenuatorfunc.cpp main.cpp qucsattenuator.cpp )

SET( attenuator_moc_headers qucsattenuator.h )

QT4_WRAP_CPP( attenuator_moc_sources ${attenuator_moc_headers} )

ADD_EXECUTABLE( qucsattenuator ${attenuator_sources} ${attenuator_moc_sources} )

TARGET_LINK_LIBRARIES( qucsattenuator ${QT_LIBRARIES} )

INSTALL (TARGETS qucsattenuator DESTINATION bin)

