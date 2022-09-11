% Octave plotting function plotFFT2V.
% for calculating and displaying FFTs of transient simulation data.
% Two data variable form.
%
% Function format
% plotFFT2V(Type, Xlabel,  Xstart, Xstop,
%		Y1Name, Y1label, Y1Colour, 
%		Y2Name, Y2label, Y2Colour, Linewidth, FTime)
%
% Parameters
% Type				Plot type (string);  "Line" or "Stem".
% Xlabel		        X axis label (string).
% Xstart			X axis start value (real).
% Xstop				X axis stop value (real).
% Y1Name			Y1 variable name (string).
% Y1label			Y1 axis label (string).
% Y1Colour			Y1 plot colour (string).
% Y2Name			Y2 variable name (string).
% Y2label			Y2 axis label (string).
% Y2Colour			Y2 plot colour (string).
% Linewidth			Thickness of plotted line in pixels (integer).
% FTime				Transient simulation FTime (real).
%
% Function written with Octave version >= 3.2.4
% Copyright 2012 by Mike Brinson
% Published under GNU General Public License (GPL V2). 
% No warranty at all.
% 
function [Y1, Y2, freq] = plotFFT2V(Type, Xlabel, Xstart, Xstop,
				     Y1Name, Y1label,Y1Colour,
				     Y2Name, Y2label,Y2Colour, Linewidth, FTime)
 if (nargin < 12)
     error("Too few input parameters." )
     return
  endif
  if (nargout > 3)
   error("Too many output parameters." )
   return
  endif
  clf;
        subplot( 2,1,1)
  	N1 = length(Y1Name);
  	Y1 = abs(fft(Y1Name))/N1;
  	N1O2 = ceil(N1/ 2);
  	Yt1(1) = Y1(1);
  	Yt1(2:N1O2) = 2*Y1(2:N1O2);
  	freq = linspace( 0, N1O2, N1O2)/FTime; 
  	if strcmp(Type, "Line") 
         	plot(freq, Yt1, "linewidth", Linewidth, "color", Y1Colour);
        else 
           	stem(freq, Yt1, "linewidth", Linewidth, "color", Y1Colour);
        endif
        axis([Xstart, Xstop]);
 	xlabel(Xlabel, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
        ylabel(Y1label, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");

        subplot( 2,1,2) 
	N2 = length(Y2Name);
  	Y2 = abs(fft(Y2Name))/N2;
  	N2O2 = ceil(N2/ 2);
  	Yt2(1) = Y2(1);
  	Yt2(2:N2O2) = 2*Y2(2:N2O2);
  	if strcmp(Type, "Line") 
         	plot(freq, Yt2, "linewidth", Linewidth, "color", Y2Colour);,
        else 
          	stem(freq, Yt2, "linewidth", Linewidth, "color", Y2Colour);
        endif
        axis([Xstart, Xstop]);
 	xlabel(Xlabel, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
  	ylabel(Y2label, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
endfunction
  
