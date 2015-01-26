# qucs/extsimkernels library
# Classes and GUI for external simulation kernels (ngspice, xyce, etc.) interfacing

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(EXTSIMKERNELS_HDRS
externsimdialog.h
abstractspicekernel.h
ngspice.h
xyce.h
qucs2spice.h
)

SET(EXTSIMKERNELS_SRCS
externsimdialog.cpp
abstractspicekernel.cpp
ngspice.cpp
xyce.cpp
qucs2spice.cpp
)

SET(EXTSIMKERNELS_MOC_HDRS
externsimdialog.h
abstractspicekernel.h
ngspice.h
xyce.h
)

QT4_WRAP_CPP( EXTSIMKERNELS_MOC_SRCS ${EXTSIMKERNELS_MOC_HDRS} )

ADD_LIBRARY(extsimkernels ${EXTSIMKERNELS_HDRS} ${EXTSIMKERNELS_SRCS} ${EXTSIMKERNELS_MOC_SRCS})

