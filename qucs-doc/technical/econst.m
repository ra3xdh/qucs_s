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
% The function computes the error constants, order and steps for the
% given linear multistep integration method characterized by the 'a'
% and 'b' coefficients.
%

function constants = econst (b, a)

  steps = 0;
  for i = 1:length(a) % determine number of steps
    if (b(i) != 0 || a(i) != 0)
      steps++;
    endif
  endfor
  printf("%d-step, ", steps-1);

  p = 0;
  for i = 1:length(a) % determine highest coefficient index
    if (b(length(a)-i+1) != 0 || a(length(a)-i+1) != 0)
      p = length(a) - i;
      break;
    endif
  endfor

  coeff = 0;          % check order of method
  q = 0;
  while (abs(coeff) <= 1e-12)
    coeff = 0;
    if (q == 0)
      % special C0 constant
      for i = -1:p-1
	coeff = coeff - a(i+2);
      endfor
    elseif (q == 1)
      % special C1 constant
      n = p - 1;
      for i = -1:n
	coeff = coeff - b(i+2) * (n - i)^(q-1);
      endfor
      for i = -1:n
	coeff = coeff - a(i+2) * (n - i)^(q);
      endfor      
    else
      % usual Cq coefficient
      f = 1;
      for i = 1:q-1
	f = f * i;
      endfor
      n = p - 1;
      for i = -1:n-1
	coeff = coeff - 1 / f * b(i+2) * (n - i)^(q-1);
      endfor
      f = f * q;
      for i = -1:n-1
	coeff = coeff - 1 / f * a(i+2) * (n - i)^(q);
      endfor
    endif
    constants(++q) = coeff;
  endwhile
  printf("order %d, error constant %g\n", q-2, coeff);
endfunction
