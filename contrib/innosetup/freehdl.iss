;
; freehdl.iss - inno Setup script file
;
<<<<<<< HEAD
; Copyright (C) 2006, 2008 Stefan Jahn <stefan@lkcc.org>
=======
; Copyright (C) 2006, 2008, 2009, 2011 Stefan Jahn <stefan@lkcc.org>
>>>>>>> 80028cb8206ee83926db69b5bd20c9a3c932403d
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

;
; NOTE: additional files: bin/libregex.dll lib/libregex.a
;

<<<<<<< HEAD
#define RELEASE "0.0.7"
#define BASENAME "freehdl"
#define APPNAME "FreeHDL"
#define APPVERNAME "FreeHDL 0.0.7 binary package for Win32"
=======
#define RELEASE "0.0.8"
#define BASENAME "freehdl"
#define APPNAME "FreeHDL"
#define APPVERNAME "FreeHDL 0.0.8 binary package for Win32"
>>>>>>> 80028cb8206ee83926db69b5bd20c9a3c932403d
#define URL "http://www.freehdl.seul.org"
#define TREE "C:\home\freetree"

[Setup]
AppName={# APPNAME}
AppVerName={# APPVERNAME}
AppPublisher=the Qucs team
AppPublisherURL={# URL}
AppSupportURL={# URL}
AppUpdatesURL={# URL}
DefaultDirName={pf}\FreeHDL
DefaultGroupName=FreeHDL
AllowNoIcons=yes
LicenseFile={# TREE}\gpl.rtf
OutputBaseFilename={# BASENAME}-{# RELEASE}-setup
Compression=lzma
SolidCompression=yes
ChangesEnvironment=yes

[Registry]
; Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: FREEHDL; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381
; Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: MINGWDIR; ValueData: {code:MinGWDir}; Flags: createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381
Root: HKCU; Subkey: Environment; ValueType: string; ValueName: FREEHDL; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist; MinVersion: 0,4.00.1381
Root: HKCU; Subkey: Environment; ValueType: string; ValueName: MINGWDIR; ValueData: {code:MinGWDir}; Flags: createvalueifdoesntexist; MinVersion: 0,4.00.1381

[Tasks]
; Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{# TREE}\bin\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\include\*"; DestDir: "{app}\include"; Flags: ignoreversion recursesubdirs createallsubdirs
; Source: "{# TREE}\info\*"; DestDir: "{app}\info"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\lib\*"; DestDir: "{app}\lib"; Flags: ignoreversion recursesubdirs createallsubdirs
; Source: "{# TREE}\man\*"; DestDir: "{app}\man"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\share\*"; DestDir: "{app}\share"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\misc\*"; DestDir: "{app}\misc"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Visit the FreeHDL Web Site"; Filename: "{app}\misc\website.url"
Name: "{group}\{cm:UninstallProgram,FreeHDL}"; Filename: "{uninstallexe}"

[Run]

[Code]
function MinGWDir(Param: String): String;
var Dir : String;
var Found : Boolean;
begin
  Found := False;
  Dir := GetEnv('MINGWDIR');
  if DirExists (Dir) then
  begin
    if FileExists(Dir + '\bin\g++.exe') then
    begin
      Found := True;
    end;
  end;

  while not Found do
  begin
    BrowseForFolder('Please select a directory where MinGW32 including its C/C++ compiler ' +
                    'is installed, then click OK.', Dir, False);
    if DirExists (Dir) then
    begin
      if FileExists(Dir + '\bin\g++.exe') then
      begin
        Found := True;
      end;
    end;
  end;

  Result := Dir;
end;

