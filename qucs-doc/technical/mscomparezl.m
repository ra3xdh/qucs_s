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

eopen ("mscomparezl.eps",ePageOrientation,ePageWidth,ePageHeight);

% apply plot rectangle area
ePlotAreaPos=[22 15];
ePlotAreaHeight=H;
ePlotAreaWidth=W;

% set grid options
eXGridVisible=1; eXGridDash=1;
eYGridVisible=1; eYGridDash=1;

% set axis layout
eXAxisNorthValueVisible=0;
eXAxisSouthScale=[0 1 5];
eYAxisEastValueVisible=0;
eYAxisWestScale=[-2 .5 1.5];
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

u = linspace(0.01,5.01,101);
er = 9.8;

eXAxisSouthLabelText="normalised strip width W/h";
eYAxisWestLabelText="deviation of impedance ZL in percent";
eAxesLabelTextFont=1;

% apply legend layout
ePlotLegendPos=[90 93];
lb=[2.8 1.9-0.5 4.9 1.1-0.4];
bb=[lb(1) lb(2); lb(3) lb(2); lb(3) lb(4); lb(1) lb(4); lb(1) lb(2)];
eplot(bb(:,1),bb(:,2),"",-1,[1 1 1]);

a = 1 + 1 / 49 * log ((u.^4 + (u / 52).^2) / (u.^4 + 0.432));
a = a + 1 / 18.7 * log (1 + (u / 18.1).^3);
b = 0.564 * ((er - 0.9) / (er + 3))^0.053;
g = (er + 1) / 2 + (er - 1) / 2 * (1 + 10 ./ u).^(-a * b);
f = 6 + (2 * M_PI - 6) * exp (-(30.666 ./ u).^0.7528);
d = Z0 / 2 / M_PI * log (f ./ u + sqrt (1 + (2 ./ u).^2));
zl = d ./ sqrt (g);
zlref = zl;

eplot (u,(zl - zlref)./zlref * 100,"Hammerstad and Jensen",1,[0 1 0],1);

g = (er + 1) / 2 + (er - 1) / 2 * (1 + 10 ./ u).^(-0.5);
for i = 1 : length (u)
  if (u(i) <= 1)
    z = Z0 / M_PI / 2 * log (8 / u(i) + u(i) / 4);
  else
    z = Z0 / (u(i) + 2.42 - 0.44 / u(i) + (1 - 1 / u(i))^(6));
  endif
  zl(i) = z / sqrt (g(i));
endfor
zl2 = zl;
eplot (u,(zl2 - zlref)./zlref * 100,"Schneider",2,[0 0 1],1);

for i = 1 : length (u)
  if (u(i) <= 3.3)
    c = log (4 / u(i) + sqrt ((4 / u(i))^2 + 2));
    z = c - 0.5 * (er - 1) / (er + 1) * (log (M_PI_2) + log (2 * M_2_PI) / er);
    z = z * Z0 / M_PI / sqrt (2 * (er + 1));
  else
    c = u(i) / 2 + log (4) / M_PI;
    c = c + (er + 1) / 2 / M_PI / er * log (M_PI_2 * e * (u(i) / 2 + 0.94));
    c = c + (er - 1) / 2 / M_PI / er^2 * log (e * M_PI^2 / 16);
    z = Z0 / 2 / sqrt (er) / c;
    %z = z + 0.098456; % minimal correction
  endif
  zl(i) = z;
endfor
zl3 = zl;
eplot (u,(zl3 - zlref)./zlref * 100,"Wheeler",0,[1 0 0],1);

eclose (1,0);
%eview;
