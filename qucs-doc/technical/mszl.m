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

eopen ("mszl.eps",ePageOrientation,ePageWidth,ePageHeight);

% apply plot rectangle area
ePlotAreaPos=[22 15];
ePlotAreaHeight=H;
ePlotAreaWidth=W;

% set grid options
eXGridVisible=1; eXGridDash=1;
eYGridVisible=1; eYGridDash=1;

% set axis layout
eXAxisNorthValueVisible=0;
%eXAxisSouthScale=[-1 1 1];
eYAxisEastValueVisible=0;
eYAxisWestScale=[0 50 250];
eYAxisWestLabelDistance=8;

% set axes options
eAxesValueFontSize=5;
eAxesLabelFontSize=eAxesValueFontSize;
eAxesLabelTextFont=1;
%eAxesTicShortLength=0;

% set legend options
ePlotLegendFontSize=eAxesValueFontSize;
ePlotLegendTextFont=eAxesLabelTextFont;


Z0     = 376.73031346958504364963;
M_PI_2 = 1.5707963267948966192313216916397514;
M_PI_4 = 0.7853981633974483096156608458198757;
M_PI   = 3.1415926535897932384626433832795029;
M_2_PI = 0.6366197723675813430755350534900574;
M_1_PI = 0.3183098861837906715377675267450287;

u = linspace(0.1,10.1,101);
k = [1 2.1 2.5 3.78 9.8 12.9 16];
k = [1 3.78 9.5];

eXAxisSouthLabelText="normalised strip width W/h";
eYAxisWestLabelText="impedance ZL in Ohm";
eAxesLabelTextFont=1;
eXAxisSouthScaleType=2;

for n = 1 : length (k)
  er = k(n);
  a = 1 + 1 / 49 * log ((u.^4 + (u / 52).^2) / (u.^4 + 0.432));
  a = a + 1 / 18.7 * log (1 + (u / 18.1).^3);
  b = 0.564 * ((er - 0.9) / (er + 3))^0.053;
  g = (er + 1) / 2 + (er - 1) / 2 * (1 + 10 ./ u).^(-a * b);
  f = 6 + (2 * M_PI - 6) * exp (-(30.666 ./ u).^0.7528);
  d = Z0 / 2 / M_PI * log (f ./ u + sqrt (1 + (2 ./ u).^2));
  zl = d ./ sqrt (g);
  eplot (u,zl,"",0,[0 0 0],1);
endfor

eclose (1,0);
%eview;
