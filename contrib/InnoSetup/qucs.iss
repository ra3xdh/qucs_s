; Qucs-S Inno Setup script file
; Refactored for improved readability and maintainability
;
; Copyright (C) 2005-2011 Stefan Jahn <stefan@lkcc.org>
; Copyright (C) 2014-2016 Guilherme Brondani Torri <guitorri@gmail.com>
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

#ifndef RELEASE
  #define RELEASE "24.3.0"
#endif

#ifndef APPNAME
  #define APPNAME "Qucs-S"
#endif

#define URL "https://ra3xdh.github.io/"
#define TREE "..\..\build\qucs-suite\"

[Setup]
AppName={#APPNAME}
AppVersion={#RELEASE}
AppPublisher=The Qucs-S Team
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}
DefaultDirName={pf}\Qucs-S
DefaultGroupName=Qucs-S
AllowNoIcons=yes
LicenseFile={#TREE}\misc\gpl.rtf
OutputBaseFilename={#APPNAME}-{#RELEASE}-setup
Compression=lzma2/max
SolidCompression=yes
ChangesEnvironment=yes
UsePreviousAppDir=yes
WizardStyle=modern
SetupIconFile={#TREE}\misc\qucs.ico
Uninstallable=yes
ArchitecturesInstallIn64BitMode=x64compatible

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#TREE}\bin\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#TREE}\misc\*"; DestDir: "{app}\misc"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#TREE}\lib\*"; DestDir: "{app}\lib"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#TREE}\share\*"; DestDir: "{app}\share"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Qucs-S Simulator"; Filename: "{app}\bin\qucs-s.exe"; IconFilename: "{app}\misc\qucs.ico"; WorkingDir: "{app}\bin"
Name: "{group}\Visit the Qucs Web Site"; Filename: "{app}\misc\website.url"
Name: "{group}\Technical Online Documentation"; Filename: "{app}\misc\docsite.url"
Name: "{group}\{cm:UninstallProgram,Qucs}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\Qucs-S"; Filename: "{app}\bin\qucs-s.exe"; IconFilename: "{app}\misc\qucs.ico"; WorkingDir: "{app}\bin"; Tasks: desktopicon

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode: Integer;
  Uninstall: String;
begin
  if (CurStep = ssInstall) then begin
    if RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#APPNAME}_is1', 'UninstallString', Uninstall) then begin
      MsgBox('An existing version of {#APPNAME} was detected. It will now be removed before installing the new version.', mbInformation, MB_OK);
      Exec(RemoveQuotes(Uninstall), ' /SILENT', '', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode);
    end;
  end;
end;
