classdef cppinterface < handle
    % base class for implementing interfaces to C++ classes
    %
    % 
    
    properties (SetAccess = private, Hidden = true)
        
        objectHandle = []; % Handle to the underlying C++ class instance
        
    end
    
    properties (SetAccess = private, Hidden = false)
        
        mex_interface_fcn = []; % mex handle function to the underlying C++ class instance
        
    end
    
    methods
        
        %% Constructor - Create a new C++ class instance
        function this = cppinterface(mexfcn)
            
            if isa(mexfcn, 'function_handle')
                this.mex_interface_fcn = mexfcn;
            else
                error('Supplied interface function, mexfcn, is not a valid function handle.');
            end
            
            % call the mex function with the string 'new' to inform it that
            % we wish to create a new instance of the underlying C++ class
            % for use in the lifetime of this matlab class
            this.objectHandle = this.mex_interface_fcn('new');
            
        end

        %% Destructor - Destroy the C++ class instance
        function delete(this)
            % call the mex interface function with the 'delete' keyword to
            % allow it to destroy the C++ class instance as it is no longer
            % required
            if ~isempty(this.objectHandle)
                this.mex_interface_fcn('delete', this.objectHandle);
            end
        end
        
        %% C++ Function Call
        function varargout = cppcall(this, varargin)
            
            % call the mex function with the supplied keyword and the object
            % handle
            if numel(varargin) > 1
                [varargout{1:nargout}] = this.mex_interface_fcn(varargin{1}, this.objectHandle, varargin{2:end});
            else
                [varargout{1:nargout}] = this.mex_interface_fcn(varargin{1}, this.objectHandle);
            end
            
        end
        
    end
    
end