;
; mingw.iss - inno Setup script file
;
; Copyright (C) 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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


; MinGW-w64 package (same as used for Qt 4.8.6)
; Site: http://mingw-w64.sourceforge.net/
; Download MinGW 4.8.2
; http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/4.8.2/threads-posix/dwarf/i686-4.8.2-release-posix-dwarf-rt_v3-rev3.7z
; Install (extract) MinGW into Wine
; $ 7z x i686-4.8.2-release-posix-dwarf-rt_v3-rev3.7z -o${HOME}/.wine/drive_c/
; Install to: C:\mingwtree

; cp ~/git/qucs/qucs/contrib/innosetup/gpl.rtf ${HOME}/.wine/drive_c/mingw32/
; cp -r ~/git/qucs/qucs/contrib/innosetup/misc/website-mingw ${HOME}/.wine/drive_c/mingw32/misc/website-mingw



; changelog
; - try to append mingw/bin to system PATH or user PATH

#define RELEASE "i686-4.8.2-release-posix-dwarf-rt_v3-rev3"
#define BASENAME "mingw-w64"
#define APPNAME "Mingw-w64"
#define APPVERNAME "Mingw-w64 4.8.2 for Win32"
#define URL "http://www.mingw.org"
#define TREE "C:\mingw_original\mingw32"

[Setup]
AppName={# APPNAME}
AppVerName={# APPVERNAME}
AppPublisher=the Qucs Team
AppPublisherURL={# URL}
AppSupportURL={# URL}
AppUpdatesURL={# URL}
;DefaultDirName={pf}\mingw32
DefaultGroupName=Mingw-w64
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
Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}/bin"; Flags: deletevalue createvalueifdoesntexist noerror; MinVersion: 0,4.00.1381

Root: HKCU; Subkey: Environment; ValueType: string; ValueName: "PATH"; ValueData: {app}/bin; Flags: deletevalue createvalueifdoesntexist; MinVersion: 0,4.00.1381



[Tasks]
; Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{# TREE}\bin\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\etc\*"; DestDir: "{app}\etc"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\i686-w64-mingw32\*"; DestDir: "{app}\i686-w64-mingw32"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\include\*"; DestDir: "{app}\include"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\lib\*"; DestDir: "{app}\lib"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\libexec\*"; DestDir: "{app}\libexec"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\licenses\*"; DestDir: "{app}\licenses"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\opt\*"; DestDir: "{app}\opt"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{# TREE}\share\*"; DestDir: "{app}\share"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Visit the Mingw-w64 Web Site"; Filename: "{app}\misc\mingw.url"
Name: "{group}\{cm:UninstallProgram,Mingw-w64}"; Filename: "{uninstallexe}"

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
