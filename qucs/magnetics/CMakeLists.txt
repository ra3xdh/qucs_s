# qucs/components library

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(MAGNETICS_SRCS
ja_core.cpp
winding.cpp
magcoredialog.cpp
)

SET(MAGNETICS_HDRS
ja_core.h
winding.h
magcoredialog.h
)



SET(MAGNETICS_MOC_HDRS
magcoredialog.h
)


qt6_wrap_cpp(MAGNETICS_MOC_SRCS ${MAGNETICS_MOC_HDRS})



ADD_LIBRARY(magnetics STATIC ${MAGNETICS_HDRS} ${MAGNETICS_SRCS} ${MAGNETICS_MOC_SRCS} )





