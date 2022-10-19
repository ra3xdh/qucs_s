% Octave plotting function plotFFT2V.
% for calculating and displaying FFTs of transient simulation data.
% Two data variable form.
%
% Function format
% plotFD2V(Type, XName, Xlabel,
%		Y1Name, Y1label, Y1Colour, 
%		Y2Name, Y2label, Y2Colour, Linewidth)
%
% Parameters
% Type				Plot type (string);  "Line" or "Stem".
% XName                         X variable (vector)
% Xlabel		        X axis label (string).
% Y1Name			Y1 variable (vector).
% Y1label			Y1 axis label (string).
% Y1Colour			Y1 plot colour (string).
% Y2Name			Y2 variable (vector).
% Y2label			Y2 axis label (string).
% Y2Colour			Y2 plot colour (string).
% Linewidth			Thickness of plotted line in pixels (integer).
%
% Function written with Octave version >= 3.2.4
% Copyright 2012 by Mike Brinson
% Published under GNU General Public License (GPL V2). 
% No warranty at all.
% 
function plotFD2V(Type, XName, Xlabel,
		  Y1Name, Y1label,Y1Colour,
		  Y2Name, Y2label,Y2Colour, Linewidth)
 if (nargin < 10)
     error("Too few input parameters." )
     return
  endif
  clf;
        subplot( 2,1,1)
  	if strcmp(Type, "Line") 
         	plot(XName, Y1Name, "linewidth", Linewidth, "color", Y1Colour);
        else 
           	stem(XName, Y1Name, "linewidth", Linewidth, "color", Y1Colour);
        endif
        axis([XName(1), XName(end)]);
 	xlabel(Xlabel, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
        ylabel(Y1label, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");

        subplot( 2,1,2) 
  	if strcmp(Type, "Line") 
         	plot(XName, Y2Name, "linewidth", Linewidth, "color", Y2Colour);,
        else 
          	stem(XName, Y2Name, "linewidth", Linewidth, "color", Y2Colour);
        endif
        axis([XName(1), XName(end)]);
 	xlabel(Xlabel, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
  	ylabel(Y2label, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
endfunction
  
