;
; mingw.iss - inno Setup script file
;
; Copyright (C) 2006, 2008 Stefan Jahn <stefan@lkcc.org>
;
; This is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2, or (at your option)
; any later version.
;
; This software is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this package; see the file COPYING.  If not, write to
; the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
; Boston, MA 02110-1301, USA.
;

;https://sourceforge.net/projects/mingw/files/Installer/mingw-get-setup.exe
;
; MinGW32 packages:
;  * gcc-core
;  * gcc-g++
;  * binutils
;  * mingwrt-dev
;  * w32api-dev
;
;  * version gcc-g++  4.8.1-4
;
; cp ~/git/qucs/qucs/contrib/innosetup/gpl.rtf ${HOME}/.wine/drive_c/mingwtree/
; cp -r ~/git/qucs/qucs/contrib/innosetup/misc/website-mingw ${HOME}/.wine/drive_c/mingw32/misc/website-mingw
;

#define RELEASE "0.0.3"
#define BASENAME "mingw32-4.8.1"
#define APPNAME "MinGW32 C/C++"
#define APPVERNAME "MinGW32 C/C++ binary package for Win32"
#define URL "http://www.mingw.org"
#define TREE "C:\mingwtree"

[Setup]
AppName={# APPNAME}
AppVerName={# APPVERNAME}
AppPublisher=the Qucs team
AppPublisherURL={# URL}
AppSupportURL={# URL}
AppUpdatesURL={# URL}
;DefaultDirName={pf}\MinGW32
;DefaultGroupName=MinGW32
AllowNoIcons=yes
LicenseFile={# TREE}\gpl.rtf
OutputBaseFilename={# BASENAME}-{# RELEASE}-setup
Compression=lzma
SolidCompression=yes
ChangesEnvironment=yes

; no admin right required http://www.kinook.com/blog/?p=53
PrivilegesRequired=none
DefaultDirName={code:DefDirRoot}\mingw32

[Registry]
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: MINGWDIR; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381

Root: HKCU; Subkey: Environment; ValueType: string; ValueName: MINGWDIR; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist; MinVersion: 0,4.00.1381


[Tasks]
; Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{# TREE}\bin\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\include\*"; DestDir: "{app}\include"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\lib\*"; DestDir: "{app}\lib"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\libexec\*"; DestDir: "{app}\libexec"; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "{# TREE}\man\*"; DestDir: "{app}\man"; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "{# TREE}\info\*"; DestDir: "{app}\info"; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "{# TREE}\share\*"; DestDir: "{app}\share"; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "{# TREE}\misc\*"; DestDir: "{app}\misc"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Visit the MinGW32 Web Site"; Filename: "{app}\misc\website.url"
Name: "{group}\{cm:UninstallProgram,MinGW32}"; Filename: "{uninstallexe}"

[Run]



[Code]
function IsRegularUser(): Boolean;
  begin
    Result := not (IsAdminLoggedOn or IsPowerUserLoggedOn);
  end;

function DefDirRoot(Param: String): String;
  begin
    if IsRegularUser then
      Result := ExpandConstant('{localappdata}')
    else
      Result := ExpandConstant('{pf}')
  end;

