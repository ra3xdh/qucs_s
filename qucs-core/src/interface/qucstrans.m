classdef qucstrans < qucs
    % interface to the qucs transient circuit solver
    %

    % Copyright Richard Crozier 2013, 2014

    properties ( SetAccess = protected, GetAccess = public )

        isinitialised; % for marking the solver as initialised

    end

    methods
        
        %% Constructor - Create a new instance of the class
        function this = qucstrans(netlist)
            
            this = this@qucs(@m_trsolver_interface_mex, netlist);
            
            this.isinitialised = false;
            
        end

        %%%%% class methods
        
%         function debug(this)
%             this.cppcall('debug');
%         end
%         
%         function printx(this)
%             this.cppcall('printx');
%         end
        
        function voltage = getnodev (this, wirelabel)
            % gets the value of the voltage at a named node/wire
            % 
            % Syntax
            %
            % voltage = getvprobe (wirelabel)
            %
            % Input
            %
            %  wirelabel - name of the node/wire from which to obtain the
            %    data. To get data from a subcircuit node, you can
            %    reference it using the following notation:
            %
            %    SUB1.SUB2.wirelabel
            %
            %    where SUB1 is the name of the subcircuit element in the
            %    top level schematic/netlist statements which contains
            %    subcircuit SUB2, and SUB2 is the name of the subcircuit
            %    element containing the named probe. 
            %
            %    An error will be thrown if the probe is not found.
            %    
            % Output
            % 
            %  voltage - the voltage reported by the probe
            %
            
            voltage = this.cppcall ('getnodev', wirelabel);
        end

        function voltage = getvprobe (this, vprobename)
            % gets the value of a voltage probe
            % 
            % Syntax
            %
            % voltage = getvprobe (vprobename)
            %
            % Input
            %
            %  vprobename - name of the probe from which to obtain the
            %    data. Gets the voltage reported by the probe with the
            %    given name. To get data from a subcircuit probe, you can
            %    reference it using the following notation:
            %
            %    SUB1.SUB2.vprobename
            %
            %    where SUB1 is the name of the subcircuit element in the
            %    top level schematic/netlist statements which contains
            %    subcircuit SUB2, and SUB2 is the name of the subcircuit
            %    element containing the named probe.
            %
            %    An error will be thrown if the probe is not found.
            %    
            % Output
            % 
            %  voltage - the voltage reported by the probe
            %
            
            voltage = this.cppcall ('getvprobe', vprobename);
        end

        function current = getiprobe (this, iprobename)
            % gets the value of a current probe
            % 
            % Syntax
            %
            % current = getiprobe (iprobename)
            %
            % Input
            %
            %  iprobename - name of the probe from which to obtain the
            %    data. Gets the current reported by the probe with the
            %    given name. To get data from a subcircuit probe, you can
            %    reference it using the following notation:
            %
            %    SUB1.SUB2.iprobename
            %
            %    where SUB1 is the name of the subcircuit element in the
            %    top level schematic/netlist statements which contains
            %    subcircuit SUB2, and SUB2 is the name of the subcircuit
            %    element containing the named probe.
            %
            %    An error will be thrown if the probe is not found.
            %    
            % Output
            % 
            %  current - the current reported by the probe
            %
            
            current = this.cppcall ('getiprobe', iprobename);
        end
        
        function setecvs (this, name, voltage)
            % sets the value of an externally controlled voltage source
            
            this.cppcall ('setecvs', name, voltage);
        end
        
    end
    
end