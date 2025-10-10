# qucs/extsimkernels library
# Classes and GUI for external simulation kernels (ngspice, xyce, etc.) interfacing

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} )

# Add ngspice include directory if shared library support is enabled
if(WITH_NGSPICE_SHARED AND NGSPICE_INCLUDE_DIR)
    INCLUDE_DIRECTORIES(${NGSPICE_INCLUDE_DIR})
endif()

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
s2spice.h
spicelibcompdialog.h
#xspice_cmbuilder.h
#codemodelgen.h
CdlSettingsDialog.h
)

# Add ngspice_shared header if enabled
if(WITH_NGSPICE_SHARED)
    list(APPEND EXTSIMKERNELS_HDRS ngspice_shared.h)
endif()

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
CdlNetlistWriter.cpp
s2spice.cpp
spicelibcompdialog.cpp
#xspice_cmbuilder.cpp
#codemodelgen.cpp
CdlSettingsDialog.cpp
)

# Add ngspice_shared source if enabled
if(WITH_NGSPICE_SHARED)
    list(APPEND EXTSIMKERNELS_SRCS ngspice_shared.cpp)
endif()

SET(EXTSIMKERNELS_MOC_HDRS
externsimdialog.h
abstractspicekernel.h
ngspice.h
xyce.h
customsimdialog.h
simsettingsdialog.h
spicelibcompdialog.h
CdlSettingsDialog.h
)

# Add ngspice_shared to MOC headers if enabled
if(WITH_NGSPICE_SHARED)
    list(APPEND EXTSIMKERNELS_MOC_HDRS ngspice_shared.h)
endif()

QT6_WRAP_CPP( EXTSIMKERNELS_MOC_SRCS ${EXTSIMKERNELS_MOC_HDRS} )


ADD_LIBRARY(extsimkernels STATIC ${EXTSIMKERNELS_HDRS} ${EXTSIMKERNELS_SRCS} ${EXTSIMKERNELS_MOC_SRCS})

# Link against ngspice shared library if enabled
if(WITH_NGSPICE_SHARED AND NGSPICE_LIBRARY)
    target_link_libraries(extsimkernels PUBLIC ${NGSPICE_LIBRARY})
    message(STATUS "Linking extsimkernels with ngspice shared library: ${NGSPICE_LIBRARY}")
endif()

ADD_SUBDIRECTORY( xspice )

