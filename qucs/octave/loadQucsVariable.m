% -*- texinfo -*-
% @deftypefn {Function File} {[@var{x}] =} loadQucsVariable (@var{filename}, @var{varname})
% Read the vector @var{x} from a Qucs dataset file @var{filename}.
% The variable name in the Qucs dataset must be named @var{varname}.
%
% Example
% @example
% [x] = loadQucsVariable ("actest.dat", "out.v")
% x = 
%   1 + 0.5i
%   2 + 1.5i
% @end example
%
% @end deftypefn

% Script written with Octave 3.2.4
% Copyright 2010 by Michael Margraf (michael.margraf@alumni.tu-berlin.de)
% Published under GNU General Public License (GPL V2). No warranty at all.

function [x] = loadQucsVariable(filename, varName)
    x = [];
    data = loadQucsDataSet(filename);
    x = getQucsVariable(data, varName);
