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

eopen ("stabmoulton.eps",ePageOrientation,ePageWidth,ePageHeight);

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
eYAxisWestScale=[-3 1 3];
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
eXAxisSouthScale=[-5.5 1 4.6];

p = linspace (0, 2*pi, 100);
z = exp (i * p);

x = [-8 8]; y = [0 0];
eplot (x, y,"",0,[0 0 0],1);
y = [-5 5]; x = [0 0];
eplot (x, y,"",0,[0 0 0],1);

output_precision = 12;

Moulton1 = [1 0; -1 1] ./ 1;
area = zpoly (Moulton1(1,:), Moulton1(2,:), z);
printf("AM1 error constants: ");
disp(econst (Moulton1(1,:), Moulton1(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Moulton2 = [1 1; -2 2] ./ 2;
area = zpoly (Moulton2(1,:), Moulton2(2,:), z);
printf("AM2 error constants: ");
disp(econst (Moulton2(1,:), Moulton2(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Moulton3 = [5 8 -1; -12 12 0] ./ 12;
area = zpoly (Moulton3(1,:), Moulton3(2,:), z);
printf("AM3 error constants: ");
disp(econst (Moulton3(1,:), Moulton3(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Moulton4 = [9 19 -5 1; -24 24 0 0] ./ 24;
area = zpoly (Moulton4(1,:), Moulton4(2,:), z);
printf("AM4 error constants: ");
disp(econst (Moulton4(1,:), Moulton4(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Moulton5 = [251 646 -264 106 -19; -720 720 0 0 0] ./ 720;
area =  zpoly (Moulton5(1,:), Moulton5(2,:), z);
printf("AM5 error constants: ");
disp(econst (Moulton5(1,:), Moulton5(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

Moulton6 = [475 1427 -798 482 -173 27; -1440 1440 0 0 0 0] ./ 1440;
area = zpoly (Moulton6(1,:), Moulton6(2,:), z);
printf("AM6 error constants: ");
disp(econst (Moulton6(1,:), Moulton6(2,:)));
eplot (real (area), imag (area),"",0,[0 0 1],0.5);

eclose (1,0);
%eview;
