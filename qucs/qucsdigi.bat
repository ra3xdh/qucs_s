@echo off

REM
REM qucsdigi.bat - wrapper script for digital simulation
REM
REM Copyright (C) 2005, 2006, 2009, 2011 Stefan Jahn <stefan@lkcc.org>
REM
REM This is free software; you can redistribute it and/or modify
REM it under the terms of the GNU General Public License as published by
REM the Free Software Foundation; either version 2, or (at your option)
REM any later version.
REM 
REM This software is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM GNU General Public License for more details.
REM 
REM You should have received a copy of the GNU General Public License
REM along with this package; see the file COPYING.  If not, write to
REM the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
REM Boston, MA 02110-1301, USA.  
REM

if "X%6"=="X" goto usage

set NAME=%1
set NAMEOUT=%2
set TIME=%~3
set DIR=%4
set BINDIR=%5
set VLIBS=%6
set OPTION=%7

if not exist "%DIR%" goto nodir

cd /d "%DIR%"

if not exist %NAME% goto nofile

copy %NAME% digi.vhdl > NUL
set NAME=digi

set CXX=g++
set CXXFLAGS=-O2 -I"%FREEHDL%/include"
set LDFLAGS=-L"%FREEHDL%/lib" -L"%FREEHDL%/lib/freehdl" -Wl,--enable-auto-import -s
set LIBS=-lfreehdl-kernel -lfreehdl-std -lieee -lregex

set PATH=%PATH%;%FREEHDL%/bin;%MINGWDIR%/bin;%QUCSDIR%/bin

echo running C++ conversion...
freehdl-v2cc -m %NAME%._main_.cc -L"%FREEHDL%/share/freehdl/lib" -Lvhdl -o %NAME%.cc %NAME%.vhdl

echo compiling functions...
%CXX% %CXXFLAGS% -c %NAME%.cc

echo compiling main...
%CXX% %CXXFLAGS% -c %NAME%._main_.cc

echo linking...
%CXX% %NAME%._main_.o %NAME%.o %LDFLAGS% -Lvhdl %VLIBS% %LIBS% -o %NAME%.exe

echo simulating...
%NAME%.exe -q -cmd "dc -f %NAME%.vcd -t 1 ps -q;d;run %TIME%;q;" < NUL

echo running VCD conversion...
qucsconv %OPTION% -if vcd -of qucsdata -i %NAME%.vcd -o %NAMEOUT%

goto end

:usage
echo Usage: %0 "<netlist.txt> <output.dat> <time> <directory> <bindirectory> <vlibs> [<convoption>]"
echo Directory has to contain the file 'netlist.txt'.
exit /b 1
goto end

:nodir
echo %DIR%: Not a directory
exit /b 1
goto end

:nofile
echo %NAME%: No such file
exit /b 1
goto end

:end
