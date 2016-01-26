
; AutoIt script to automate (silently) the installation of Qt.
; wine ~/.wine/drive_c/Program\ Files/AutoIt3/AutoIt3.exe autoQt.au3 

ShellExecute("qt-opensource-windows-x86-mingw482-4.8.6.exe")
WinWaitActive("Qt OpenSource 4.8.6 Setup", "Setup Wizard")
Send("!n")

WinWaitActive("Qt OpenSource 4.8.6 Setup", "Open Source Edition")
Send("!n")

WinWaitActive("Qt OpenSource 4.8.6 Setup", "License Agreement")
Send("{TAB}")
Send("{UP}")
Send("{SPACE}")
Send("!n")

WinWaitActive("Qt OpenSource 4.8.6 Setup", "Choose Components")
Send("!n")

WinWaitActive("Qt OpenSource 4.8.6 Setup", "Choose Install Location")
Send("!n")

WinWaitActive("Qt OpenSource 4.8.6 Setup", "Choose Start Menu Folder")
Send("!n")

WinWaitActive("Qt OpenSource 4.8.6 Setup", "MinGW Installation")
Send("{ENTER}")
Send("{ENTER}")

WinWaitActive("Qt OpenSource 4.8.6 Setup", "Installation Complete")
Send("!n")

WinWaitActive("Qt OpenSource 4.8.6 Setup", "Completing")
Send("{SPACE}")
Send("{DOWN}")
Send("{SPACE}")
Send("{ENTER}")

