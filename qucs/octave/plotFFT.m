% Octave data plotting function
% for calculating and displaying FFTs of transient simulation data.
% Single data variable form.
%
% Function format
% [Y, freq} = plotFFT(Type, VName, Xlabel,Xstart, Xstop, Ylabel,YColour, Linewidth, FTime)
%
% Parameters
% Type				Plotted line style (string); Type = "Line" line style plot,
%                               any other string stem style plot.
% VName				Plotted variable name (string).
% Xlabel			X axis label (string).
% Xstart			X start value (real).
% Xstop				X stop value  (real).
% Ylabel			Y axis label (string.
% YColour			Colour of plot (string), for example "red".
% Linewidth			Thickness of plotted line or stem curvein pixels (integer).
% FTime				Transient simulation finish time in seconds (Float).
%
% Function written with Octave  >= version 3.2.4
% Copyright 2012 by Mike Brinson
% Published under GNU General Public License (GPL V2). 
% No warranty at all.
% 
function [Y, freq] = plotFFT(Type, VName, 
			     Xlabel, Xstart, Xstop,
                             Ylabel, YColour, 
                             Linewidth, FTime)
  if(nargin < 9)
     error("Too few input parameters.")
     return
  endif
  if (nargout > 2)
   error("Too many output parameters.")
   return
  endif
  clf;
  N = length(VName);
  Y = abs(fft(VName))/N;
  NO2 = ceil(N/2);
  Yt(1) = Y(1);
  Yt(2:NO2) = 2*Y(2:NO2);
  freq = linspace(0, NO2, NO2)/FTime;
  if strcmp(Type, "Line") 
       plot(freq, Yt, "linewidth", Linewidth, "color", YColour);,
  else 
       stem(freq, Yt, "linewidth", Linewidth, "color", YColour);
  endif
  axis([Xstart, Xstop]);
 	xlabel(Xlabel, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
  ylabel(Ylabel, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
endfunction