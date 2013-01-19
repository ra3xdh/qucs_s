% Octave plotting function plotTimeFFT.
% for calculating FFT and displaying both transient simulation
% data and FFT.
%
% Function format
% plotTimeFFT(Type, XName, Yname, Xlabel, Ylabel, Xstart, Xstop,
%	      Linewidth, FTime, Freqmin, Freqmax))
%
% Parameters
% Type                          FFT plot type: "line" or "stem"
% XName				X variable name (string).
% Yname				Y variable name (string).
% Xlabel		        X axis label (string).
% Ylabel		        Y axis label (string).
% Xstart			X axis start value (real).
% Xstop				X axis stop value (real).
% Linewidth			Thickness of plotted line in pixels (integer).
% FTime				Transient simulation FTime (real)
% Freqmin                       FFT plot minimum frequeny (Hz).
% Freqmax         		FFT plot maximum frequency (Hz).
%
% Function written with Octave version >= 3.2.4
% Copyright 2012 by Mike Brinson
% Published under GNU General Public License (GPL V2). 
% No warranty at all.
% 
function [Y, freq] = plotTimeFFT(Type, XName,YName,  Xlabel, Ylabel,  Xstart, Xstop,
			         Linewidth, FTime, Freqmin, Freqmax)
 if (nargin < 9)
     error("Too few input parameters." )
     return
 endif
 if (nargout > 2)
   error("Too many output parameters." )
   return
 endif
 clf;
        subplot(2,1,1)
  	plot(XName, YName, "linewidth", Linewidth );
        axis([Xstart, Xstop]);
 	xlabel(Xlabel, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
        ylabel(Ylabel, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");

        subplot(2,1,2) 
	N = length(YName);
  	Y = abs(fft(YName))/N;
  	NO2 = ceil(N/2);
  	Yt(1) = Y(1);
  	Yt(2:NO2) = 2*Y(2:NO2);
        freq = linspace( 0, NO2, NO2)/FTime; 
  	if strcmp(Type, "line") 
         	plot(freq, Yt, "linewidth", Linewidth);,
        else 
          	stem(freq, Yt, "linewidth", Linewidth);
        endif
        
        axis([Freqmin, Freqmax]);
 	xlabel("Frequency (Hz)", "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
  	ylabel(Ylabel, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
endfunction
  
