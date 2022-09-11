function qucstrsolversetup(dodebug, verbose)
% compiles the qucstrsolver mexfunction interface

    if nargin < 1
        dodebug = false;
    end
    
    if nargin < 2
        verbose = false;
    end 

    if isoctave
        cc.Name = 'gcc';
    else
        try
            % First check a C++ compiler is present, and selected
            cc = mex.getCompilerConfigurations('C++', 'Selected');
        catch
            % if the getCompilerConfigurations call fails, try with gcc,
            % assuming that we are on windows and perhaps using gnumex
            cc.Name = 'gcc';
        end
    end

    % store the current directory
    origdir = pwd;

    % change to the qucstrsolversetup directory (the directory this file is in)
    cd(fileparts(which('qucstrsolversetup.m')));

    % set some common compiler flags, we replace all calls to printf to
    % calls to mexPrintf
    if dodebug
        common_compiler_flags = {'-DHAVE_CONFIG_H', '-g', '-D"_MEX_DEBUG"'};
    else
        common_compiler_flags = {'-DHAVE_CONFIG_H'};% common_compiler_flags = '-D"printf=mexPrintf"';
    end
    
    if verbose
        common_compiler_flags = [common_compiler_flags, {'-v'}];
    end

    % add link commands for the installed qucs shared library
    libcommands = { ...
                   '-lqucsator', ...
                   '-I../..', ... % temp top level directory containing config.h, get rid of in future
                  };

    % put all the compiler commands in a cell array
    mexcommands = [ common_compiler_flags, ...
                    { ...
                      'm_trsolver_interface_mex.cpp', ...
                      'mextrsolver.cpp', ...
                      ... %'-fpermissive', ...
                    }, ...
                    libcommands ...
                  ];

    if isoctave
        mkoctfile('--mex', mexcommands{:});
    else
        % call mex with the appropriately constructed commands
        mex(mexcommands{:});
    end

    % return to original directory
    cd(origdir);

end
