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

#define RELEASE "0.0.17"
#define BASENAME "qucs"
#define APPNAME "Qucs"
#define APPVERNAME "Quite Universal Circuit Simulator 0.0.17 binary package for Win32"
#define URL "http://qucs.sourceforge.net"
#define TREE "c:\qucs-git\release\qucs-win32-bin\"

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
Root: HKCU; Subkey: Environment; ValueName: "Path"; ValueType: "string"; ValueData: "{app}\bin;{olddata}"; Check: NotOnPathAlready(); Flags: preservestringtype;


[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{# TREE}\bin\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
; Source: "{# TREE}\man\*"; DestDir: "{app}\man"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\share\*"; DestDir: "{app}\share"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\misc\*"; DestDir: "{app}\misc"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: "{# TREE}\iverilog-0.9.6_setup.exe"; DestDir: "{tmp}"
Source: "{# TREE}\freehdl-0.0.8-setup.exe"; DestDir: "{tmp}"
Source: "{# TREE}\mingw32-g++-0.0.2-setup.exe"; DestDir: "{tmp}"

[Icons]
Name: "{group}\Quite Universal Circuit Simulator"; Filename: "{app}\bin\qucs.exe"; IconFilename: "{app}\misc\qucs64x64.ico"; WorkingDir: "{app}\bin"
Name: "{group}\Visit the Qucs Web Site"; Filename: "{app}\misc\website.url"
Name: "{group}\Technical Online Documentation"; Filename: "{app}\misc\docsite.url"
Name: "{group}\{cm:UninstallProgram,Qucs}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\Qucs"; Filename: "{app}\bin\qucs.exe"; IconFilename: "{app}\misc\qucs64x64.ico"; WorkingDir: "{app}\bin"; Tasks: desktopicon

[Run]
Filename: "{tmp}\iverilog-0.9.6_setup.exe"; Parameters: ""; Check: ShouldInstallVerilog 
Filename: "{tmp}\mingw32-g++-0.0.2-setup.exe"; Parameters: ""; Check: ShouldInstallMingw
Filename: "{tmp}\freehdl-0.0.8-setup.exe"; Parameters: ""; Check: ShouldInstallFreehdl 

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



function NotOnPathAlready(): Boolean;
var
  BinDir, Path: String;
begin
  Log('Checking if Gtk2Hs\bin dir is already on the %PATH%');
  if RegQueryStringValue(HKEY_CURRENT_USER, 'Environment', 'Path', Path) then
  begin // Successfully read the value
    Log('HKCU\Environment\PATH = ' + Path);
    BinDir := ExpandConstant('{app}\bin');
    Log('Looking for Gtk2Hs\bin dir in %PATH%: ' + BinDir + ' in ' + Path);
    if Pos(LowerCase(BinDir), Lowercase(Path)) = 0 then
    begin
      Log('Did not find Gtk2Hs\bin dir in %PATH% so will add it');
      Result := True;
    end
    else
    begin
      Log('Found Gtk2Hs bin dir in %PATH% so will not add it again');
      Result := False;
    end
  end
  else // The key probably doesn't exist
  begin
    Log('Could not access HKCU\Environment\PATH so assume it is ok to add it');
    Result := True;
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  BinDir, Path: String;
begin
  if (CurUninstallStep = usPostUninstall)
     and (RegQueryStringValue(HKEY_CURRENT_USER, 'Environment', 'PATH', Path)) then
  begin
    BinDir := ExpandConstant('{app}\bin');
    if Pos(LowerCase(BinDir) + ';', Lowercase(Path)) <> 0 then
    begin
      StringChange(Path, BinDir + ';', '');
      RegWriteStringValue(HKEY_CURRENT_USER, 'Environment', 'PATH', Path);
    end;
  end;
end;



function ShouldInstallVerilog: Boolean;
begin
  Result := False;
  // Ask the user a Yes/No question
  if MsgBox('Install iverilog?', mbConfirmation, MB_YESNO) = IDYES then
  begin
    Result := True;
  end;


end;

function ShouldInstallFreehdl: Boolean;
begin
  Result := False;
  if MsgBox('Install Freehdl?', mbConfirmation, MB_YESNO) = IDYES then
  begin
    Result := True;
  end;
end;

function ShouldInstallMingw: Boolean;
begin
  Result := False;
  if MsgBox('Install Mingw32? This package is required if you want to install Freehdl', mbConfirmation, MB_YESNO) = IDYES then
  begin
    Result := True;
  end;
end;


