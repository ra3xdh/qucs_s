# qucs/qt3_compat library

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(QT3_COMPAT_HDRS
q3frame.h
q3gdict.h
q3ptrcollection.h
q3ptrdict.h
q3scrollview.h
qt_compat.h
)

SET(QT3_COMPAT_SRCS
q3frame.cpp
q3gdict.cpp
q3scrollview.cpp
)

SET(QT3_COMPAT_MOC_HDRS
q3scrollview.h
)

QT5_WRAP_CPP( QT3_COMPAT_SRCS ${QT3_COMPAT_MOC_HDRS} )

ADD_LIBRARY(qt3_compat ${QT3_COMPAT_HDRS} ${QT3_COMPAT_SRCS})
