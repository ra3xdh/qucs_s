PROJECT(qucs-suite CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
cmake_policy(VERSION 2.8)

FIND_PACKAGE( Qt4 REQUIRED )

SET( QT_USE_QT TRUE )
#SET( QT_USE_QTXML TRUE )
#SET( QT_USE_QTSVG TRUE )
SET( QT_USE_QTCORE TRUE )
SET( QT_USE_QTGUI TRUE )
SET( QT_USE_QT3SUPPORT TRUE )

SET( QT_DEBUG TRUE)

INCLUDE( ${QT_USE_FILE} )

ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/qucs
        #${CMAKE_SOURCE_DIR}/bitmaps ->no sources here
        ${CMAKE_SOURCE_DIR}/components
        ${CMAKE_SOURCE_DIR}/diagrams
        ${CMAKE_SOURCE_DIR}/dialogs
        #${CMAKE_SOURCE_DIR}/octave ->no sources here
        ${CMAKE_SOURCE_DIR}/paintings
        )

ADD_SUBDIRECTORY( qucs )
ADD_SUBDIRECTORY( qucs-attenuator )
#ADD_SUBDIRECTORY( qucs-doc )
ADD_SUBDIRECTORY( qucs-edit )
ADD_SUBDIRECTORY( qucs-filter )
#ADD_SUBDIRECTORY( qucs-filter-v2 )  # v2 not completed?
ADD_SUBDIRECTORY( qucs-help )
ADD_SUBDIRECTORY( qucs-lib )
ADD_SUBDIRECTORY( qucs-transcalc )
#ADD_SUBDIRECTORY( examples )
ADD_SUBDIRECTORY( contrib )



SET(LANGUAGES ar;ca;cs;de;es;fr;he;hu;it;jp;kk;pl;pt;ro;ru;sv;tr;uk)
SET(LANGUAGE_TS_FILES)
FOREACH(LANGUAGE ${LANGUAGES})
 SET(TS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/qucs/qucs_${LANGUAGE}.ts")
 SET(LANGUAGE_TS_FILES ${LANGUAGE_TS_FILES} ${TS_FILE})
 SET_SOURCE_FILES_PROPERTIES(${TS_FILE} PROPERTIES OUTPUT_LOCATION
   "${CMake_BINARY_DIR}${CMAKE_DATA_DIR}")
ENDFOREACH(LANGUAGE ${LANGUAGES})
IF(LANGUAGE_TS_FILES)
 QT4_CREATE_TRANSLATION(qm_files ${SRCS} ${LANGUAGE_TS_FILES})
ENDIF(LANGUAGE_TS_FILES)


#
# Custom uninstall target
#
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)


