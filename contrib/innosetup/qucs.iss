;
; qucs.iss - inno Setup script file
;
; Copyright (C) 2005-2011 Stefan Jahn <stefan@lkcc.org>
; Copyright (C) 2014 Guilherme Brondani Torri <guitorri@gmail.com>
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

; changelog
; - add infobefore about path with spaces
; - commented out non admin install, doesn't seem to work
; - enable non admin install

#define RELEASE "0.0.18"
#define BASENAME "qucs"
#define APPNAME "Qucs"
#define APPVERNAME "Quite Universal Circuit Simulator 0.0.18 binary package for Win32"
#define URL "http://qucs.sourceforge.net"
#define TREE "c:\qucs-win32-bin\"
#define octaveversion "3.6.4"

#define freehdl  "freehdl-0.0.8-1-setup.exe"
#define iverilog "iverilog-20130827_setup.exe"
#define mingw    "mingw-w64-i686-4.8.2-release-posix-dwarf-rt_v3-rev3-setup.exe"


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
InfoBeforeFile={# TREE}\infobefore.rtf
UsePreviousAppDir=yes

; no admin right required http://www.kinook.com/blog/?p=53
;PrivilegesRequired=none
;DefaultDirName={code:DefDirRoot}\Qucs

[Registry]
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: QUCSDIR; ValueData: "{app}"; Flags: deletevalue createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: HOME; ValueData: "{code:HomeDir}"; Flags: createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: ADMSXMLBINDIR; ValueData: "{app}\bin"; Flags: deletevalue createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: ASCOBINDIR; ValueData: "{app}\bin"; Flags: createvalueifdoesntexist noerror ; MinVersion: 0,4.00.1381

Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: OCTAVEDIR; ValueData: {app}\share\qucs\octave; Flags: deletevalue createvalueifdoesntexist noerror uninsclearvalue; MinVersion: 0,4.00.1381
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueName: "Path"; ValueType: "string"; ValueData: "{app}\bin;{olddata}"; Check: NotOnPathAlready(); Flags: preservestringtype noerror;
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueName: "Path"; ValueType: "string"; ValueData: "{code:OctaveDir};{olddata}"; Tasks: octave; Check: OctaveNotOnPathAlready(); Flags: preservestringtype noerror;

Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: OCTAVEBINDIR; ValueData: "{code:GetOctaveBinDir}"; Flags: createvalueifdoesntexist noerror ; MinVersion: 0,4.00.1381

; handle Current User install
;Root: HKCU; Subkey: Environment; ValueType: string; ValueName: QUCSDIR; ValueData: {app}; Flags: deletevalue createvalueifdoesntexist noerror uninsdeletekey; MinVersion: 0,4.00.1381
;Root: HKCU; Subkey: Environment; ValueType: string; ValueName: HOME; ValueData: {code:HomeDir}; Flags: createvalueifdoesntexist noerror uninsdeletekey; MinVersion: 0,4.00.1381
;Root: HKCU; Subkey: Environment; ValueType: string; ValueName: ADMSXMLBINDIR; ValueData: {app}\bin; Flags: deletevalue createvalueifdoesntexist noerror uninsdeletekey; MinVersion: 0,4.00.1381
;Root: HKCU; Subkey: Environment; ValueType: string; ValueName: ASCOBINDIR; ValueData: {app}\bin; Flags: deletevalue createvalueifdoesntexist noerror uninsdeletekey; MinVersion: 0,4.00.1381


[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "mingw32"; Description: "Install Mingw-w64 4.8.2 (Required for FreeHDL, Verilog-A)"; GroupDescription: "Install bundled software"; Flags: checkedonce
Name: "freehdl"; Description: "Install FreeHDL 0.0.8"; GroupDescription: "Install bundled software"; Flags: checkedonce
Name: "iverilog"; Description: "Install iverilog dev-130827"; GroupDescription: "Install bundled software"; Flags: checkedonce
Name: "octave"; Description: "Download Octave"; GroupDescription: "Install bundled software"; Flags: checkedonce


[Files]
Source: "{# TREE}\bin\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\include\*"; DestDir: "{app}\include"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\lib\*"; DestDir: "{app}\lib"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\misc\*"; DestDir: "{app}\misc"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\share\*"; DestDir: "{app}\share"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: "{# TREE}\{# iverilog}"; DestDir: "{tmp}"
Source: "{# TREE}\{# freehdl}"; DestDir: "{tmp}"
Source: "{# TREE}\{# mingw}"; DestDir: "{tmp}"

[Icons]
Name: "{group}\Quite Universal Circuit Simulator"; Filename: "{app}\bin\qucs.exe"; IconFilename: "{app}\misc\qucs64x64.ico"; WorkingDir: "{app}\bin"
Name: "{group}\Visit the Qucs Web Site"; Filename: "{app}\misc\website.url"
Name: "{group}\Technical Online Documentation"; Filename: "{app}\misc\docsite.url"
Name: "{group}\{cm:UninstallProgram,Qucs}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\Qucs"; Filename: "{app}\bin\qucs.exe"; IconFilename: "{app}\misc\qucs64x64.ico"; WorkingDir: "{app}\bin"; Tasks: desktopicon

[Run]
Filename: "{tmp}\{# mingw}"; Parameters: ""; Tasks: mingw32
Filename: "{tmp}\{# freehdl}"; Parameters: ""; Tasks: freehdl
Filename: "{tmp}\{# iverilog}"; Parameters: ""; Tasks: iverilog


[Code]
// globar var, set by Octavedir, used to create OCTAVEBINDIR used by Qucs
var
  OctaveBinDir: String;

// get Octave bin dir
function GetOctaveBinDir(Param: String): String;
begin
  Result := OctaveBinDir;
end;

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
  Log('Checking if Qucs\bin dir is already on the %PATH%');
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path) then
  begin // Successfully read the value
    Log('HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment\PATH = ' + Path);
    BinDir := ExpandConstant('{app}\bin');
    Log('Looking for Qucs\bin dir in %PATH%: ' + BinDir + ' in ' + Path);
    if Pos(LowerCase(BinDir), Lowercase(Path)) = 0 then
    begin
      Log('Did not find Qucs\bin dir in %PATH% so will add it');
      Result := True;
    end
    else
    begin
      Log('Found Qucs bin dir in %PATH% so will not add it again');
      Result := False;
    end
  end
  else // The key probably doesn't exist
  begin
    Log('Could not access HKCU\Environment\PATH so assume it is ok to add it');
    Result := True;
  end;
end;


function OctaveNotOnPathAlready(): Boolean;
var
  BinDir, Path: String;
begin
  Log('Checking if octave-{# octaveversion}\bin dir is already on the %PATH%');
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path) then
  begin // Successfully read the value
    Log('HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment\PATH = ' + Path);
    BinDir := ExpandConstant('{pf}\octave-{# octaveversion}\bin');
    Log('Looking for octave-{# octaveversion}\bin dir in %PATH%: ' + BinDir + ' in ' + Path);
    if Pos(LowerCase(BinDir), Lowercase(Path)) = 0 then
    begin
      Log('Did not find octave-{# octaveversion}\bin dir in %PATH% so will add it');
      Result := True;
    end
    else
    begin
      Log('Found octave-{# octaveversion} bin dir in %PATH% so will not add it again');
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
  if MsgBox('A browser will be opened to download octave-{# octaveversion} Setup', mbConfirmation, MB_YESNO) = IDYES then
  begin
  ShellExec('open', 'http://sourceforge.net/projects/octave/files/Octave%20Windows%20binaries/Octave%203.6.4%20for%20Windows%20Microsoft%20Visual%20Studio/octave-3.6.4-vs2010-setup.exe/download',
      '', '', SW_SHOW, ewNoWait, ErrCode);
  end;
end;


function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;
  if CurPageID = wpSelectTasks then
  begin
    // why is this the position 6??
    if WizardForm.TasksList.Checked[6] then
      DownloadOctave;
  end;
end;

function OctaveDir(Param: String): String;
var Dir : String;
var Found : Boolean;
begin
  Found := False;

    BrowseForFolder('Please select a directory where octave ' +
                    'is installed, then click OK.', Dir, False);
    if DirExists (Dir) then
    begin
      if FileExists(Dir + '\bin\octave.exe') then
      begin
        Found := True;
      end;
    end;
  if Found = False then
  begin
    Dir := 'c:\Software\octave{# octaveversion}\bin';
  end
  else
  begin
    Dir := Dir + '\bin';
  end;

  // push to global var
  OctaveBinDir := Dir;
  Result := Dir;
end;

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




