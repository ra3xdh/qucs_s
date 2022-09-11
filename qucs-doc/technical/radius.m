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

H=100; W=100*4/3;

eglobpar;                % access to global parameters
ePageOrientation=0;      % set page orientation

ePageWidth=W+24;         % set page size and ratio
ePageHeight=H+15;
eWinWidth=ePageWidth;
eWinHeight=ePageHeight;

eopen ("radius.eps",ePageOrientation,ePageWidth,ePageHeight);

% apply plot rectangle area
ePlotAreaPos=[18 10];
ePlotAreaHeight=H;
ePlotAreaWidth=W;

% set grid options
eXGridVisible=1; eXGridDash=1;
eYGridVisible=1; eYGridDash=1;

% set axis layout
eXAxisNorthValueVisible=0;
eXAxisSouthScale=[0.6 0.02 0.7];
eYAxisEastValueVisible=0;
eYAxisWestScale=[-0.5 0.2 1];

% set axes options
eAxesValueFontSize=7;
eAxesLabelTextFont=1;
%eAxesTicShortLength=0;

% set legend options
ePlotLegendFontSize=eAxesValueFontSize;
ePlotLegendTextFont=eAxesLabelTextFont;

Is = 1e-13;
Ut = 25e-3;

u  = linspace(0.6,0.7,100);
i0 = Is * (exp(u/Ut) - 1);
i1 = Is/Ut * exp(u/Ut);
i2 = Is/Ut/Ut * exp(u/Ut);
i3 = Is/Ut/Ut/Ut * exp(u/Ut);
z0 = linspace(0,0,100);

r0 = ((1 + i1.^2).^(3/2)) ./ i2;
r1 = (((3/2)*((1+i1.^2).^(1/2))*2.*i1) ./ i2);
r1 = r1 .+ (((1+i1.^2).^(3/2)) ./ (i2.^2));

r1 = i2 .* 3/2.*(1+i1.^2).^(1/2)*2.*i1.*i2 - (1+i1.^2).^(3/2) .* i3;
r1 = r1./(i2.^2);
r1 = r1./20;

% apply legend layout
ePlotLegendPos=[5 90];
lb=[0.602 0.95 0.675 0.64];
bb=[lb(1) lb(2); lb(3) lb(2); lb(3) lb(4); lb(1) lb(4); lb(1) lb(2)];
eplot(bb(:,1),bb(:,2),"",-1,[1 1 1]);

eplot (u,r0,"curve radius",0,[1 0 0],1);
eplot (u,r1,"derivative of curve radius",1,[0 1 0],1);
eplot (u,z0,"",0,[0 0 1],1);

eclose (1,0);
%eview;
