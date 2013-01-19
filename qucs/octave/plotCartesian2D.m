% Octave plotting function plotCartesian2D.
% Single data variable form.
%
% Function format
% plotCarteaian2D(Type, XName, YName, Xlabel, Ylabel, Xstart, Xstop, Linewidth)
%
% Parameters
% Type				type (string);  "semilogx", or "semilogy" or "loglog" else "plot".
% XName				X variable name (string).
% YName				Y variable name (string).
% Xlabel		        X axis label (string).
% Ylabel		        Y axis label (string).
% Xstart			X axis start value (real).
% Xstop				X axis stop value (real).
% Linewidth			Thickness of plotted line in pixels (integer).
%
% Function written with Octaveversion >= 3.2.4
% Copyright 2012 by Mike Brinson
% Published under GNU General Public License (GPL V2). 
% No warranty at all.
% 
%
function plotCartesian2D(Type, XName, YName, Xlabel, Ylabel, 
                         Xstart, Xstop, Linewidth)
  if(nargin < 8)
     error("Too few input parameters.")
  return
  endif
  clf;
  if strcmp(Type, "semilogx") 
       semilogx(XName, YName, "linewidth", Linewidth);,
  elseif strcmp(Type, "semilogy") 
       semilogy(XName, Yname, "linewidth", Linewidth);
  elseif strcmp(Type,"loglog")   
         loglog(XName, YName, "linewidth", Linewidth);
  else  strcmp(Type, "plot")
         plot(XName, YName, "linewidth", Linewidth );
  endif 
  axis([Xstart, Xstop]);
	xlabel(Xlabel, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
        ylabel(Ylabel, "fontsize", 14, "fontname", "Arial", "fontweight", "bold");
endfunction
  