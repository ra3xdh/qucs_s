% -*-octave-*-
% 
% Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
%
% This is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2, or (at your option)
% any later version.
% 
% This software is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with this package; see the file COPYING.  If not, write to
% the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
% Boston, MA 02110-1301, USA.  
%

H=100; W=H*4/3+25;

eglobpar;                % access to global parameters
ePageOrientation=0;      % set page orientation

ePageWidth=W+40;         % set page size and ratio
ePageHeight=H+25;
eWinWidth=ePageWidth;
eWinHeight=ePageHeight;

eopen ("stabgearex.eps",ePageOrientation,ePageWidth,ePageHeight);

% apply plot rectangle area
ePlotAreaPos=[22 15];
ePlotAreaHeight=H;
ePlotAreaWidth=W;

% set grid options
eXGridVisible=1; eXGridDash=1;
eYGridVisible=1; eYGridDash=1;

% set axis layout
eXAxisNorthValueVisible=0;
eYAxisEastValueVisible=0;
eYAxisWestScale=[-5 1 5];
eYAxisWestLabelDistance=8;

% set axes options
eAxesValueFontSize=5;
eAxesLabelFontSize=eAxesValueFontSize;
eAxesLabelTextFont=1;
%eAxesTicShortLength=0;

% set legend options
ePlotLegendFontSize=eAxesValueFontSize;
ePlotLegendTextFont=eAxesLabelTextFont;

eXAxisSouthLabelText="Re(lh)";
eYAxisWestLabelText="Im(lh)";
eAxesLabelTextFont=1;
eXAxisSouthScale=[-8 1 8];

p = linspace (0, 2*pi, 400);
z = exp (i * p);

x = [-8 8]; y = [0 0];
eplot (x, y,"",0,[0 0 0],1);
y = [-5 5]; x = [0 0];
eplot (x, y,"",0,[0 0 0],1);

output_precision = 12;

Gear1 = [0 0 0; -1 2 -1];
area = zpoly (Gear1(1,:), Gear1(2,:), z);
printf("GEAR1 error constants: ");
disp(econst (Gear1(1,:), Gear1(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Gear2 = [0 0 0 0; -1 3 -3 1];
area = zpoly (Gear2(1,:), Gear2(2,:), z);
printf("GEAR2 error constants: ");
disp(econst (Gear2(1,:), Gear2(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Gear3 = [0 0 0 0 0; -1 4 -6 4 -1];
area = zpoly (Gear3(1,:), Gear3(2,:), z);
printf("GEAR3 error constants: ");
disp(econst (Gear3(1,:), Gear3(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Gear4 = [0 0 0 0 0 0; -1 5 -10 10 -5 1];
area = zpoly (Gear4(1,:), Gear4(2,:), z);
printf("GEAR4 error constants: ");
disp(econst (Gear4(1,:), Gear4(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Gear5 = [0 0 0 0 0 0 0; -1 6 -15 20 -15 6 -1];
area = zpoly (Gear5(1,:), Gear5(2,:), z);
printf("GEAR5 error constants: ");
disp(econst (Gear5(1,:), Gear5(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Gear6 = [0 0 0 0 0 0 0 0; -1 7 -21 35 -35 21 -7 1];
area = zpoly (Gear6(1,:), Gear6(2,:), z);
printf("GEAR6 error constants: ");
disp(econst (Gear6(1,:), Gear6(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

eclose (1,0);
%eview;
