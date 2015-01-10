# qucs/extsimkernels library
# Classes and GUI for external simulation kernels (ngspice, xyce, etc.) interfacing

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(EXTSIMKERNELS_HDRS
ngspicesimdialog.h
)

SET(EXTSIMKERNELS_SRCS
ngspicesimdialog.cpp
)

SET(EXTSIMKERNELS_MOC_HDRS
ngspicesimdialog.h
)

QT4_WRAP_CPP( EXTSIMKERNELS_MOC_SRCS ${EXTSIMKERNELS_MOC_HDRS} )

ADD_LIBRARY(extsimkernels ${EXTSIMKERNELS_HDRS} ${EXTSIMKERNELS_SRCS} ${EXTSIMKERNELS_MOC_SRCS})

