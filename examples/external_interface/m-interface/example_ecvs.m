% example_probe_and_subcircuit_access_async.m
%
% 

thisfilepath = fileparts(which('example_probe_and_subcircuit_access_async.m'));

netlistfile = fullfile(thisfilepath, '..', 'example_ecvs.net');


%% solve the circuit (asynchronously) and extract data

ecvsname = 'ECVS1';
iprobename = 'Pr1';

tend = 1;
tstart = 0.0;
delta = (tend - tstart) / 10;

clear qtr_async1

% create a new asynchronous solver object from a netlist
qtr_async1 = asynctrcircuit (netlistfile);

qtr_async1.init(tstart, delta/10);

ecvsvoltage(1) = sin(2 * pi * 1.0 * tstart);

qtr_async1.setecvs (ecvsname, ecvsvoltage(1));
    
current(1) = qtr_async1.getiprobe (iprobename);

% choose some time points
t = linspace(tstart, tend, 50)';

for ind = 2:numel(t)
    
    ecvsvoltage(ind) = sin(2 * pi * 1.0 * t(ind));
    
    qtr_async1.setecvs (ecvsname, ecvsvoltage(ind));
    
    qtr_async1.stepsolve (t(ind));

    current(ind,1) = qtr_async1.getiprobe (iprobename);

    % the solution must be explicitly accepted for it to become a
    % part of the circuit solution history
    qtr_async1.acceptstep ();

end

% plot the voltages and currents extracted during simulation
figure; 
[AX, H1, H2] = plotyy ( t, ...
                        ecvsvoltage, ...
                        t, ...
                        current );

legend ([H1;H2], ecvsname, iprobename);

title ('Example Externally Controlled Voltage Source', 'FontSize', 14)
xlabel('Time (s)', 'FontSize', 14)
set(get(AX(1),'Ylabel'),'String','Voltage [V]', 'FontSize', 14) 
set(get(AX(2),'Ylabel'),'String','Current [A]', 'FontSize', 14) 

