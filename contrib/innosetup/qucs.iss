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
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: QUCSDIR; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: HOME; ValueData: {code:HomeDir}; Flags: createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: ASCODIR; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueName: "Path"; ValueType: "string"; ValueData: "{app}\bin;{olddata}"; Check: NotOnPathAlready(); Flags: preservestringtype;
;Root: HKCU; Subkey: Environment; ValueType: string; ValueName: QUCSDIR; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist; MinVersion: 0,4.00.1381
;Root: HKCU; Subkey: Environment; ValueType: string; ValueName: HOME; ValueData: {code:HomeDir}; Flags: createvalueifdoesntexist; MinVersion: 0,4.00.1381
;Root: HKCU; Subkey: Environment; ValueType: string; ValueName: ASCODIR; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist; MinVersion: 0,4.00.1381
;Root: HKCU; Subkey: Environment; ValueName: "Path"; ValueType: "string"; ValueData: "{app}\bin;{olddata}"; Check: NotOnPathAlready(); Flags: preservestringtype;


[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "iverilog"; Description: "Install iverilog 0.9.6"; GroupDescription: "Install bundled software"; Flags: checkedonce
Name: "mingw32"; Description: "Install Mingw32 0.0.2 (Required for FreeHDL)"; GroupDescription: "Install bundled software"; Flags: checkedonce
Name: "freehdl"; Description: "Install FreeHDL 0.0.8"; GroupDescription: "Install bundled software"; Flags: checkedonce
Name: "octave"; Description: "Download Octave"; GroupDescription: "Install bundled software"; Flags: checkedonce


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
Filename: "{tmp}\iverilog-0.9.6_setup.exe"; Parameters: ""; Tasks: iverilog 
Filename: "{tmp}\mingw32-g++-0.0.2-setup.exe"; Parameters: ""; Tasks: mingw32
Filename: "{tmp}\freehdl-0.0.8-setup.exe"; Parameters: ""; Tasks: freehdl


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

function DownloadOctave: Boolean;
var
  ErrCode: Integer;
begin
  MsgBox('A browser will be opened to download Octave 3.6.4 Setup', mbConfirmation, MB_OK);
  Result := True;
  ShellExec('open', 'http://sourceforge.net/projects/octave/files/Octave%20Windows%20binaries/Octave%203.6.4%20for%20Windows%20Microsoft%20Visual%20Studio/octave-3.6.4-vs2010-setup.exe/download',
      '', '', SW_SHOW, ewNoWait, ErrCode);
end;


function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;
  if CurPageID = wpSelectTasks then
  begin
    if WizardForm.TasksList.Checked[5] then
      DownloadOctave;
  end;
end;






