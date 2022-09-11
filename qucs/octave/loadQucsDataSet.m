function dataSet = loadQucsDataSet(dataSetFile)
% Load a dataset produced by the Qucs circuit simulator.
%
% Syntax
%
% dataSet = loadQucsDataSet(dataSetFile)
%
% Input
%
%  dataSetFile - name of the data file produced by Qucs.
%
% Output
%
%  dataSet - an array of structures containing the information from the
%    Qucs data file. Each structure contains the following fields:
%
%    name       | The name of the variable associated with the data in the 
%               | 'data' field of the structure, e.g. 
%               |
%    nameDep    | The name of the dependent variable associated with the
%               | data, e.g. for a time series simulation this will be
%               | 'time' and another structure will hold the time data as a
%               | variable.
%               | 
%    dep        | Value of 0 or 1 specifying if the data in the data field 
%               | is dependent on another variable. 
%               |
%    data       | Vector of values containing the data for the specified
%               | variable
%

% -*- texinfo -*-
% @deftypefn {Function File} {[@var{data}] =} loadQucsDataSet (@var{filename})
% Loads the Qucs dataset from the Qucs dataset file @var{filename}.
% @end deftypefn

% Copyright 2010 (C) 2010 by Eberhard Gamm <eberhard.gamm@gmx.de>
% Modified  2012 Richard Crozier
% Published under GNU General Public License (GPL V2). No warranty at all.

    dataSet = [];
    fid = fopen(dataSetFile,'r');
    if fid < 0
        fprintf(1,'Data set %s not found\n',dataSetFile);
	    return;
    end

    error = 0;
    idata = 0;
    idx = 0;
    while ~feof(fid) && ~error
        line = fgetl(fid);
        if ~ischar(line)
            break;
        end
        if isempty(line)
            break;
        end
        ll = length(line);
        if line(1) == '<'
            if strcmp(line(1:min(7,ll)),'<indep ')
%             [name,len,cnt] = sscanf(line(8:end-1),'%s %d','C');
                out = textscan(line(8:end-1),'%s %d');
                if numel(out) ~= 2
                    fprintf(1,'Invalid independent variable in data set %s\n',dataSetFile);
                    error = 1;
                    break;
                end
                name = out{1}{1};
                numdatapoints = out{2};
                idx = idx + 1;
                dataSet(idx).name = name;
                dataSet(idx).nameDep = '-';
                dataSet(idx).dep = 0;
                dataSet(idx).data = zeros(1,numdatapoints);
                idata = 1;
            end
            if strcmp(line(1:min(5,ll)),'<dep ')
%             [name,nameDep,cnt] = sscanf(line(6:end-1),'%s %s','C');
                out = textscan(line(5:end-1),'%s %s');
                if numel(out) ~= 2
                    fprintf(1,'Invalid dependent variable in data set %s\n',dataSetFile);
                    error = 1;
                    break;
                end
                name = out{1}{1};
                nameDep = out{2}{1};
                idx = idx + 1;
                dataSet(idx).name = name;
                dataSet(idx).nameDep = nameDep;
                dataSet(idx).dep = 1;
                dataSet(idx).data = zeros(1,numdatapoints);
                idata = 1;
            end
            if strcmp(line(1:min(8,ll)),'</indep>') || strcmp(line(1:min(6,ll)),'</dep>')
                dataSet(idx).data = dataSet(idx).data(1:idata-1);
                dataSet(idx).len = idata - 1;
                idata = 0;
            end
        else
            if idata > 0
                % check if number is complex
                jloc = strfind(line, 'j');
                if isempty(jloc)
                    [val,cnt] = sscanf(line,'%f');
                    if cnt ~= 1
                        error('QUCS:laoddataset:badnum','Invalid data in data set %s\n', dataSetFile);
                    end
                else
                    line(jloc) = line(jloc-1);
                    line(jloc-1) = ' ';
                    [val,cnt] = sscanf(line,'%f');
                    if cnt ~= 2
                        error('QUCS:laoddataset:badnum', 'Invalid data in data set %s\n', dataSetFile);
                    end
                    val = complex(val(1), val(2));
                end
                
%                 if idata > length(dataSet(idx).data)
%                     dataSet(idx).data = [dataSet(idx).data zeros(1,10000)];
%                 end
                dataSet(idx).data(idata) = val;
                idata = idata + 1;
            end
        end
    end
    fclose(fid);

end
