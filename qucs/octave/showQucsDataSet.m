function showQucsDataSet(dataSet)
% Dssplay the contents of Qucs data set to the command line

% -*- texinfo -*-
% @deftypefn {Function File} showQucsDataSet (@var{dataset})
% Shows the content of the Qucs dataset @var{dataset}.
% @end deftypefn

% Copyright 2010 (C) 2010 by Eberhard Gamm <eberhard.gamm@gmx.de>
% Published under GNU General Public License (GPL V2). No warranty at all.
    
	if isempty(dataSet)
        fprintf(1,'\nData set is empty.\n\n');
    else
        fprintf(1,'\nContents of data set:\n');
        fprintf(1,'------------------------------------------------------------\n');
        fprintf(1,'Variable                 Dependency                   Length\n');
        fprintf(1,'------------------------------------------------------------\n');
        for i = 1:length(dataSet)
            fprintf(1,'%-24s %-24s %10d\n',dataSet(i).name,...
                dataSet(i).nameDep,dataSet(i).len);
        end
        fprintf(1,'------------------------------------------------------------\n\n');
    end

end
