function data = getQucsVariable(dataSet,name)
% Get the specified variable from Qucs data set
%
% Syntax
%
% data = getQucsVariable(dataSet,name)
%
% Input
%
% dataSet - a qucs dataset in the format produced by loadQucsDataSet.m
%
% name - a string containing the name of the variable to be 
%   extracted from the data set
%
% Output
%
% data - vector of values containing the variable data
%
%
% Example
%
% [x] = getQucsVariable (data, "out.v")
% x = 
%   1 + 0.5i
%   2 + 1.5i
%

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

end
