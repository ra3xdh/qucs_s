# qucs/dialogs library

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} ) # ui_*.h, out of tree

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(DIALOGS_SRCS
changedialog.cpp		packagedialog.cpp vasettingsdialog.cpp
digisettingsdialog.cpp		qucssettingsdialog.cpp
importdialog.cpp		savedialog.cpp labeldialog.cpp
searchdialog.cpp     librarydialog.cpp settingsdialog.cpp
matchdialog.cpp			simmessage.cpp newprojdialog.cpp
sweepdialog.cpp			exportdialog.cpp loaddialog.cpp
)

SET(DIALOGS_MOC_HDRS
changedialog.h
digisettingsdialog.h
exportdialog.h
importdialog.h
labeldialog.h
librarydialog.h
loaddialog.h
matchdialog.h
newprojdialog.h
packagedialog.h
qucssettingsdialog.h
savedialog.h
searchdialog.h
settingsdialog.h
simmessage.h
sweepdialog.h
sweepdialog.h
vasettingsdialog.h
)

SET(DIALOGS_UIC_HDRS
searchdialog.ui
)

QT4_WRAP_CPP( DIALOGS_MOC_SRCS ${DIALOGS_MOC_HDRS} )

QT4_WRAP_UI( DIALOGS_UIC_SRCS ${DIALOGS_UIC_HDRS} )

ADD_LIBRARY(dialogs ${DIALOGS_SRCS} ${DIALOGS_MOC_SRCS} ${DIALOGS_UIC_SRCS})
