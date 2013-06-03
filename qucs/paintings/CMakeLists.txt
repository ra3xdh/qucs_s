# qucs/paintings library

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

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

QT4_WRAP_CPP( PAINTINGS_MOC_SRCS ${PAINTINGS_MOC_HDRS} )

ADD_LIBRARY(paintings ${PAINTINGS_SRCS} ${PAINTINGS_MOC_SRCS} )

