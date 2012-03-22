% -*- texinfo -*-
% @deftypefn {Function File} {[@var{x}] =} getQucsVariable (@var{dataset}, @var{varname})
% Load the vector @var{x} from a Qucs dataset @var{dataset} with the variable name @var{varname}.
%
% Example
% @example
% [x] = getQucsVariable (data, "out.v")
% x = 
%   1 + 0.5i
%   2 + 1.5i
% @end example
% @end deftypefn

% Copyright 2010 (C) 2010 by Eberhard Gamm <eberhard.gamm@gmx.de>
% Published under GNU General Public License (GPL V2). No warranty at all.

function data = getQucsVariable(dataSet,name)
% Get variable from data set

data = [];
if isempty(dataSet)
    error('Data set is empty');
end
for i=1:numel(dataSet)
    if strcmp(dataSet(i).name,name)
        data = dataSet(i).data;
        return;
    end
end
error(['Variable ' name ' not found in data set']);
