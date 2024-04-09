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
set SIMTIME=%~3
set SIMDIR=%4
set BINDIR=%5

REM echo %0 %1 %2 %3 %4 %5

REM Remove any spaces in the simulation time command.
set SIMTIME=%SIMTIME: =%
REM echo %SIMTIME%

if not exist "%SIMDIR%" goto nodir

cd /d "%SIMDIR%"

if not exist %NAME% goto nofile

copy %NAME% digi.vhdl > NUL
set NAME=digi

echo running GHDL analysis pass...
ghdl -a %NAME%.vhdl

echo running GHDL elaboration pass...
ghdl -e TestBench

echo simulating...
ghdl -r TestBench --vcd=digi.vcd --stop-time=%SIMTIME%

echo running VCD conversion...
%BINDIR%\qucsconv_rf.exe -if vcd -of qucsdata -i %NAME%.vcd -o %NAMEOUT%

goto end

:usage
echo Usage: %0 "<netlist.txt> <output.dat> <time> <directory> <bindirectory>"
echo Directory has to contain the file 'netlist.txt'.
exit /b 1
goto end

:nodir
echo %SIMDIR%: Not a directory
exit /b 1
goto end

:nofile
echo %NAME%: No such file
exit /b 1
goto end

:end
