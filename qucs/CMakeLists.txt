PROJECT(qucs CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
cmake_policy(VERSION 2.6)

SET(BITMAPDIR TRUE)
SET(BINARYDIR TRUE)
SET(DOCDIR TRUE)
SET(LANGUAGEDIR TRUE)
SET(LIBRARYDIR TRUE)
SET(OCTAVEDIR TRUE)

SET(CMAKE_BUILD_TYPE Debug)

ADD_DEFINITIONS( -DHAVE_CONFIG_H )

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

INCLUDE( ${QT_USE_FILE} )

ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE( ${QT_USE_FILE} )

ADD_SUBDIRECTORY( bitmaps )
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

ADD_EXECUTABLE( qucs ${QUCS_SRCS} ${QUCS_MOC_SRCS} )

TARGET_LINK_LIBRARIES( qucs ${QT_LIBRARIES} components diagrams dialogs paintings )

INSTALL(TARGETS qucs DESTINATION bin)


# TODO qucsdigi qucsveri qucsdigilib


# TODO rule to update Qucs strings
SET(TRANSLATIONS qucs_de.ts qucs_pl.ts qucs_fr.ts qucs_it.ts qucs_ro.ts
  qucs_pt.ts qucs_es.ts qucs_jp.ts qucs_hu.ts qucs_he.ts qucs_sv.ts
  qucs_tr.ts qucs_uk.ts qucs_ru.ts qucs_cs.ts qucs_ca.ts qucs_ar.ts
  qucs_kk.ts)

# TODO Include Qt strings
set(GENTRANSLATIONS qtgeneric_untranslated.ts qtgeneric_de.ts qtgeneric_fr.ts
  qtgeneric_es.ts qtgeneric_ro.ts qtgeneric_jp.ts qtgeneric_hu.ts
  qtgeneric_it.ts qtgeneric_he.ts qtgeneric_sv.ts qtgeneric_pl.ts
  qtgeneric_tr.ts qtgeneric_uk.ts qtgeneric_ru.ts qtgeneric_cs.ts
  qtgeneric_ca.ts qtgeneric_ar.ts qtgeneric_kk.ts)

#QT4_ADD_TRANSLATION(LANG_SRCS ${LAN_FILES})
#INSTALL( FILES ${LANG_SRCS} DESTINATION share/qucs/lang/)


