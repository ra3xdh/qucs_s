# qucs/dialogs library

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}   # ui_*.h, out of tree
)

# Header files
set(DIALOGS_HDRS
    aboutdialog.h
    changedialog.h
    digisettingsdialog.h
    exportdialog.h
    importdialog.h
    keysequenceedit.h
    labeldialog.h
    librarydialog.h
    loaddialog.h
    matchdialog.h
    newprojdialog.h
    qucssettingsdialog.h
    qucsshortcutdialog.h
    savedialog.h
    searchdialog.h
    settingsdialog.h
    simmessage.h
    sweepdialog.h
    vasettingsdialog.h
    textboxdialog.h
    displaydialog.h
    tuner.h
    fillfromspicedialog.h
)

# Source files
set(DIALOGS_SRCS
    aboutdialog.cpp
    changedialog.cpp
    vasettingsdialog.cpp
    textboxdialog.cpp
    digisettingsdialog.cpp
    qucssettingsdialog.cpp
    importdialog.cpp
    savedialog.cpp
    labeldialog.cpp
    searchdialog.cpp
    librarydialog.cpp
    settingsdialog.cpp
    matchdialog.cpp
    simmessage.cpp
    newprojdialog.cpp
    sweepdialog.cpp
    exportdialog.cpp
    loaddialog.cpp
    displaydialog.cpp
    tuner.cpp
    fillfromspicedialog.cpp
    qucsshortcutdialog.cpp
    keysequenceedit.cpp
)

# Headers that need Qt MOC generation
set(DIALOGS_MOC_HDRS
    aboutdialog.h
    changedialog.h
    digisettingsdialog.h
    exportdialog.h
    importdialog.h
    labeldialog.h
    librarydialog.h
    loaddialog.h
    matchdialog.h
    newprojdialog.h
    qucssettingsdialog.h
    savedialog.h
    searchdialog.h
    settingsdialog.h
    simmessage.h
    sweepdialog.h
    vasettingsdialog.h
    textboxdialog.h
    displaydialog.h
    tuner.h
    fillfromspicedialog.h
    qucsshortcutdialog.h
    keysequenceedit.h
)

# Qt Designer UI files
set(DIALOGS_UIC_HDRS
    searchdialog.ui
)

# Qt code generation
qt6_wrap_cpp(DIALOGS_MOC_SRCS ${DIALOGS_MOC_HDRS})
qt6_wrap_ui(DIALOGS_UIC_SRCS ${DIALOGS_UIC_HDRS})

# Build the static library
add_library(dialogs STATIC
    ${DIALOGS_HDRS}
    ${DIALOGS_SRCS}
    ${DIALOGS_MOC_SRCS}
    ${DIALOGS_UIC_SRCS}
)
