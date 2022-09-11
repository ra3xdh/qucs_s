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
% The function computes the region of absolute stability based upon the
% coefficients 'a' and 'b' characterizing an integration method.  The
% given complex vector 'z' should be the unity circle.
%

function area = zpoly (b, a, z)
  for k = 1:length (z)   % go through complex vector z
    bs = 0;
    as = 0;
    for i = 1:length(b)  % summarize denominator polynomial
      bs = bs + b(i) * z(k)^(-i);
    endfor
    for i = 1:length(a)  % summarize numerator polynomial
      as = as + a(i) * z(k)^(-i);
    endfor
    if (bs == 0)
      bs = 1;
    endif
    area(k) = - as / bs;
  endfor
endfunction
