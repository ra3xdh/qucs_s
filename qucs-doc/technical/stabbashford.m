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

eopen ("stabbashford.eps",ePageOrientation,ePageWidth,ePageHeight);

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
eYAxisWestScale=[-1.5 0.5 1.5];
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
eXAxisSouthScale=[-2.75 0.5 2.3];

p = linspace (0, 2*pi, 200);
z = exp (i * p);

x = [-8 8]; y = [0 0];
eplot (x, y,"",0,[0 0 0],1);
y = [-5 5]; x = [0 0];
eplot (x, y,"",0,[0 0 0],1);

output_precision = 12;

Bashford1 = [0 1; -1 1] ./ 1;
area = zpoly (Bashford1(1,:), Bashford1(2,:), z);
printf("AB1 error constants: ");
disp(econst (Bashford1(1,:), Bashford1(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Bashford2 = [0 3 -1; -2 2 0] ./ 2;
area = zpoly (Bashford2(1,:), Bashford2(2,:), z);
printf("AB2 error constants: ");
disp(econst (Bashford2(1,:), Bashford2(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Bashford3 = [0 23 -16 5; -12 12 0 0] ./ 12;
area = zpoly (Bashford3(1,:), Bashford3(2,:), z);
printf("AB3 error constants: ");
disp(econst (Bashford3(1,:), Bashford3(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Bashford4 = [0 55 -59 37 -9; -24 24 0 0 0] ./ 24;
area = zpoly (Bashford4(1,:), Bashford4(2,:), z);
printf("AB4 error constants: ");
disp(econst (Bashford4(1,:), Bashford4(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Bashford5 = [0 1901 -2774 2616 -1274 251; -720 720 0 0 0 0] ./ 720;
area =  zpoly (Bashford5(1,:), Bashford5(2,:), z);
printf("AB5 error constants: ");
disp(econst (Bashford5(1,:), Bashford5(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Bashford6 = [0 4277 -7923 9982 -7298 2877 -475; -1440 1440 0 0 0 0 0] ./ 1440;
area = zpoly (Bashford6(1,:), Bashford6(2,:), z);
printf("AB6 error constants: ");
disp(econst (Bashford6(1,:), Bashford6(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

eclose (1,0);
%eview;
