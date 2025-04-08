# qucs/qt3_compat library

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(QT3_COMPAT_HDRS
q3frame.h
q3scrollview.h
)

SET(QT3_COMPAT_SRCS
q3frame.cpp
q3scrollview.cpp
)

SET(QT3_COMPAT_MOC_HDRS
q3scrollview.h
q3frame.h
)


QT6_WRAP_CPP( QT3_COMPAT_SRCS ${QT3_COMPAT_MOC_HDRS} )

ADD_LIBRARY(qt3_compat STATIC ${QT3_COMPAT_HDRS} ${QT3_COMPAT_SRCS})
