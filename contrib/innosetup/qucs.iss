;
; qucs.iss - inno Setup script file
;
; Copyright (C) 2005-2011 Stefan Jahn <stefan@lkcc.org>
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

#define RELEASE "0.0.16"
#define BASENAME "qucs"
#define APPNAME "Qucs"
#define APPVERNAME "Quite Universal Circuit Simulator 0.0.16 binary package for Win32"
#define URL "http://qucs.sourceforge.net"
#define TREE "C:\home\qucstree"

[Setup]
AppName={# APPNAME}
AppVerName={# APPVERNAME}
AppPublisher=the Qucs team
AppPublisherURL={# URL}
AppSupportURL={# URL}
AppUpdatesURL={# URL}
DefaultDirName={pf}\Qucs
DefaultGroupName=Qucs
AllowNoIcons=yes
LicenseFile={# TREE}\gpl.rtf
OutputBaseFilename={# BASENAME}-{# RELEASE}-setup
Compression=lzma
SolidCompression=yes
ChangesEnvironment=yes

[Registry]
; Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: QUCSDIR; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381
; Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: HOME; ValueData: {code:HomeDir}; Flags: createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381
; Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: ASCODIR; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381
Root: HKCU; Subkey: Environment; ValueType: string; ValueName: QUCSDIR; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist; MinVersion: 0,4.00.1381
Root: HKCU; Subkey: Environment; ValueType: string; ValueName: HOME; ValueData: {code:HomeDir}; Flags: createvalueifdoesntexist; MinVersion: 0,4.00.1381
Root: HKCU; Subkey: Environment; ValueType: string; ValueName: ASCODIR; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist; MinVersion: 0,4.00.1381

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{# TREE}\bin\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
; Source: "{# TREE}\man\*"; DestDir: "{app}\man"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\share\*"; DestDir: "{app}\share"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\misc\*"; DestDir: "{app}\misc"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Quite Universal Circuit Simulator"; Filename: "{app}\bin\qucs.exe"; IconFilename: "{app}\misc\qucs64x64.ico"; WorkingDir: "{app}\bin"
Name: "{group}\Visit the Qucs Web Site"; Filename: "{app}\misc\website.url"
Name: "{group}\Technical Online Documentation"; Filename: "{app}\misc\docsite.url"
Name: "{group}\{cm:UninstallProgram,Qucs}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\Qucs"; Filename: "{app}\bin\qucs.exe"; IconFilename: "{app}\misc\qucs64x64.ico"; WorkingDir: "{app}\bin"; Tasks: desktopicon

[Run]
; Filename: "{app}\bin\qucs.exe"; Description: "{cm:LaunchProgram,Qucs}"; Flags: nowait postinstall skipifsilent

[Code]
function HomeDir(Param: String): String;
var Dir : String;
begin

  Dir := GetEnv('HOME');
  if (Dir='') then
  begin
    Dir := GetEnv('HOMEDRIVE') + GetEnv('HOMEPATH');
    if (Dir='') then
    begin
      Dir := GetEnv('USERPROFILE');
      if (Dir='') then
      begin
        Dir := 'C:\HOME';
      end;
    end;
  end;

  while not DirExists(Dir) do
  begin
    if not CreateDir(Dir) then
    begin
      MsgBox('The Setup program is unable to create the project ' #13
             'directory "' + Dir + '".', mbError, MB_OK);
      BrowseForFolder('Please select your home (project) directory, then click OK.', Dir, True);
    end;
  end;

  Result := Dir;
end;

