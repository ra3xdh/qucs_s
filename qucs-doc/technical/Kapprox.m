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

%
% The function computes the K/K' approximation in the argument
% interval [0;1].  The interval is splitted at 1/sqrt(2).
%

function k = Kapprox (x)
  for n = 1 : length(x)
    if (x(n) < 1 / sqrt(2))
      a = sqrt (1 - x(n)^2);
      k(n) = pi / (log (2 * (1 + sqrt(a)) / (1 - sqrt(a))));
    else
      a = x(n);
      k(n) = (log (2 * (1 + sqrt(a)) / (1 - sqrt(a)))) / pi;
    endif
  endfor
endfunction
