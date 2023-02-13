# qucs/paintings library

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(PAINTINGS_HDRS
arrow.h
arrowdialog.h
ellipse.h
ellipsearc.h
filldialog.h
graphicline.h
graphictext.h
graphictextdialog.h
id_dialog.h
id_text.h
painting.h
paintings.h
portsymbol.h
rectangle.h
)

SET(PAINTINGS_SRCS
arrow.cpp arrowdialog.cpp
graphicline.cpp	id_text.cpp
graphictext.cpp	painting.cpp
ellipse.cpp	graphictextdialog.cpp
portsymbol.cpp ellipsearc.cpp
rectangle.cpp filldialog.cpp
id_dialog.cpp
)

SET(PAINTINGS_MOC_HDRS
arrowdialog.h
filldialog.h
graphictextdialog.h
id_dialog.h
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

IF(QT_VERSION_MAJOR EQUAL 6)
    QT6_WRAP_CPP( PAINTINGS_MOC_SRCS ${PAINTINGS_MOC_HDRS} )
else()
    QT5_WRAP_CPP( PAINTINGS_MOC_SRCS ${PAINTINGS_MOC_HDRS} )
endif()

ADD_LIBRARY(paintings STATIC ${PAINTINGS_HDRS} ${PAINTINGS_SRCS} ${PAINTINGS_MOC_SRCS} )

