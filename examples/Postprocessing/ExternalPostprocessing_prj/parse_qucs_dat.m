function data = parse_qucs_dat(filename)
  % Parse a Qucs dataset file and return a containers.Map of {varname -> array}.
  % Keys preserve the original Qucs names, e.g. 'S[1,1]', 'frequency'.
  data = containers.Map();
  current_var = '';
  values = {};
  fid = fopen(filename, 'r');
  if fid == -1
    error('Cannot open file: %s', filename);
  end

  while ~feof(fid)
    raw = fgetl(fid);
    if ~ischar(raw), break; end
    line = strtrim(raw);

    % Independent variable
    tok = regexp(line, '^<indep\s+(\S+)\s+\d+>', 'tokens');
    if ~isempty(tok)
      current_var = tok{1}{1};
      values = {};
      continue
    end

    % Dependent variable
    tok = regexp(line, '^<dep\s+(\S+)\s+\S+>', 'tokens');
    if ~isempty(tok)
      current_var = tok{1}{1};
      values = {};
      continue
    end

    % End of block
    if strncmp(line, '</', 2)
      if ~isempty(current_var) && ~isempty(values)
        data(current_var) = cell2mat(values);
      end
      current_var = '';
      values = {};
      continue
    end

    % Data line
    if ~isempty(current_var) && ~isempty(line)
      % Convert Qucs complex notation a+/-jb -> a+/-bi
      line_oct = regexprep(line, '([+-])j([0-9eE.+\-]+)', '$1$2i');
      val = str2double(line_oct);
      if ~isnan(val)
        values{end+1} = val;
      end
    end
  end

  fclose(fid);
end
