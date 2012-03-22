% -*- texinfo -*-
% @deftypefn {Function File} {[@var{data}] =} loadQucsDataSet (@var{filename})
% Loads the Qucs dataset from the Qucs dataset file @var{filename}.
% @end deftypefn

% Copyright 2010 (C) 2010 by Eberhard Gamm <eberhard.gamm@gmx.de>
% Published under GNU General Public License (GPL V2). No warranty at all.

function dataSet = loadQucsDataSet(dataSetFile)
% Load Qucs data set

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
            [name,len,cnt] = sscanf(line(8:end-1),'%s %d','C');
            if cnt ~= 2
                fprintf(1,'Invalid independent variable in data set %s\n',dataSetFile);
                error = 1;
                break;
                error
            end
            idx = idx + 1;
            dataSet(idx).name = name;
            dataSet(idx).nameDep = '-';
            dataSet(idx).dep = 0;
            dataSet(idx).data = zeros(1,10000);
            idata = 1;
        end
        if strcmp(line(1:min(5,ll)),'<dep ')
            [name,nameDep,cnt] = sscanf(line(6:end-1),'%s %s','C');
            if cnt ~= 2
                fprintf(1,'Invalid dependent variable in data set %s\n',dataSetFile);
                error = 1;
                break;
            end
            idx = idx + 1;
            dataSet(idx).name = name;
            dataSet(idx).nameDep = nameDep;
            dataSet(idx).dep = 1;
            dataSet(idx).data = zeros(1,10000);
            idata = 1;
        end
        if strcmp(line(1:min(8,ll)),'</indep>') || strcmp(line(1:min(6,ll)),'</dep>')
            dataSet(idx).data = dataSet(idx).data(1:idata-1);
            dataSet(idx).len = idata - 1;
            idata = 0;
        end
    else
        if idata > 0
            [val,cnt] = sscanf(line,'%f');
            if cnt ~= 1
                fprintf(1,'Invalid data in data set %s\n',dataSetFile);
                error = 1;
                break;
            end
            if idata > length(dataSet(idx).data)
                dataSet(idx).data = [dataSet(idx).data zeros(1,10000)];
            end
            dataSet(idx).data(idata) = val;
            idata = idata + 1;
        end
    end
end
fclose(fid);
