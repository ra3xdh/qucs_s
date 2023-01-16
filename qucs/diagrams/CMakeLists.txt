# qucs/diagrams library

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(DIAGRAMS_HDRS
curvediagram.h
diagram.h
diagramdialog.h
diagrams.h
graph.h
marker.h
markerdialog.h
polardiagram.h
psdiagram.h
rect3ddiagram.h
rectdiagram.h
smithdiagram.h
tabdiagram.h
timingdiagram.h
truthdiagram.h
)

SET(DIAGRAMS_SRCS
curvediagram.cpp	graph.cpp		polardiagram.cpp	smithdiagram.cpp
diagram.cpp		marker.cpp		psdiagram.cpp		tabdiagram.cpp
diagramdialog.cpp	markerdialog.cpp	rect3ddiagram.cpp	timingdiagram.cpp
rectdiagram.cpp		truthdiagram.cpp
)

SET(DIAGRAMS_MOC_HDRS
diagramdialog.h
markerdialog.h
)

QT6_WRAP_CPP( DIAGRAMS_MOC_SRCS ${DIAGRAMS_MOC_HDRS} )

ADD_LIBRARY(diagrams STATIC ${DIAGRAMS_HDRS} ${DIAGRAMS_SRCS} ${DIAGRAMS_MOC_SRCS})
