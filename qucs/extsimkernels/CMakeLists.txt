# qucs/extsimkernels library
# Classes and GUI for external simulation kernels (ngspice, xyce, etc.) interfacing

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(EXTSIMKERNELS_HDRS
externsimdialog.h
abstractspicekernel.h
ngspice.h
xyce.h
qucs2spice.h
spicecompat.h
customsimdialog.h
simsettingsdialog.h
verilogawriter.h
xspice_cmbuilder.h
codemodelgen.h
)

SET(EXTSIMKERNELS_SRCS
externsimdialog.cpp
abstractspicekernel.cpp
ngspice.cpp
xyce.cpp
qucs2spice.cpp
spicecompat.cpp
customsimdialog.cpp
simsettingsdialog.cpp
verilogawriter.cpp
xspice_cmbuilder.cpp
codemodelgen.cpp
)

SET(EXTSIMKERNELS_MOC_HDRS
externsimdialog.h
abstractspicekernel.h
ngspice.h
xyce.h
customsimdialog.h
simsettingsdialog.h
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

IF(QT_VERSION_MAJOR EQUAL 6)
    QT6_WRAP_CPP( EXTSIMKERNELS_MOC_SRCS ${EXTSIMKERNELS_MOC_HDRS} )
else()
    QT5_WRAP_CPP( EXTSIMKERNELS_MOC_SRCS ${EXTSIMKERNELS_MOC_HDRS} )
endif()


ADD_LIBRARY(extsimkernels STATIC ${EXTSIMKERNELS_HDRS} ${EXTSIMKERNELS_SRCS} ${EXTSIMKERNELS_MOC_SRCS})

ADD_SUBDIRECTORY( xspice )

