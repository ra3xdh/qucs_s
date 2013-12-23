# qucs/dialogs library

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(DIALOGS_SRCS
changedialog.cpp		packagedialog.cpp vasettingsdialog.cpp
digisettingsdialog.cpp		qucssettingsdialog.cpp vtabbar.cpp
importdialog.cpp		savedialog.cpp vtabbeddockwidget.cpp
labeldialog.cpp			searchdialog.cpp vtabbutton.cpp
librarydialog.cpp		settingsdialog.cpp vtabwidget.cpp
matchdialog.cpp			simmessage.cpp newprojdialog.cpp		
sweepdialog.cpp			exportdialog.cpp
)

SET(DIALOGS_MOC_HDRS
changedialog.h
digisettingsdialog.h
exportdialog.h
importdialog.h
labeldialog.h
librarydialog.h
matchdialog.h
packagedialog.h
qucssettingsdialog.h
savedialog.h
searchdialog.h
settingsdialog.h
simmessage.h
sweepdialog.h
sweepdialog.h
vasettingsdialog.h
vtabbar.h
vtabbeddockwidget.h
vtabbutton.h
vtabwidget.h
)

QT4_WRAP_CPP( DIALOGS_MOC_SRCS ${DIALOGS_MOC_HDRS} )

ADD_LIBRARY(dialogs ${DIALOGS_SRCS} ${DIALOGS_MOC_SRCS} )
