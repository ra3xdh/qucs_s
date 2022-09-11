classdef qucs < cppinterface
   
    properties 
        
    end
    
    methods
        
        function this = qucs(mexfcn, netlistfile)
            
            % call the constructor for the parent cppinterface class with
            % the mex function for the qucs analysis to be performed
            this = this@cppinterface(mexfcn);
            
            if nargin > 1
            
                % prepare the netlist for analysis
                this.cppcall('prepare_netlist', netlistfile);
            
            end

        end
        
        function N = getn(this)
           N = this.cppcall('getN'); 
        end
        
        function M = getm(this)
           M = this.cppcall('getM'); 
        end
        
    end
    
end