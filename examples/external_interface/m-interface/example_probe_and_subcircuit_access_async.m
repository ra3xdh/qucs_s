% example_probe_and_subcircuit_access_async.m
%
% 

thisfilepath = fileparts(which('example_probe_and_subcircuit_access_async.m'));

netlistfile = fullfile(thisfilepath, '..', 'example_probe_and_subcircuit_access.net');

fprintf (1, '\nRunning example_probe_and_subcircuit_access_async ... \n');
fprintf (1, 'Demonstrates methods of extracting data during simulation.\n');
fprintf (1, 'Evaluates netlist file %s\n\n', netlistfile);

%% solve the circuit (asynchronously) and extract data

wirelabel = 'test_label';
vprobename = 'VProbe1';
iprobename = 'IProbe1';
iprobename2 = 'IProbe2';
subcktvprobe = 'SUB1.SUB1.VProbeS1';

tend = 1e-3;
tstart = 0.0;
delta = (tend - tstart) / 113.0;

clear qtr_async1

% create a new asynchronous solver object from a netlist
qtr_async1 = asynctrcircuit (netlistfile);

qtr_async1.init(tstart, delta/100);

namenodevoltage = qtr_async1.getnodev (wirelabel);

vprobevoltage = qtr_async1.getvprobe (vprobename);

iprobecurrent = qtr_async1.getiprobe (iprobename);

iprobe2current = qtr_async1.getiprobe (iprobename2);

subcktvprobevoltage = qtr_async1.getvprobe (subcktvprobe);

% choose some time points
t = linspace(tstart, tend, 50)';

for ind = 2:numel(t)

    qtr_async1.stepsolve (t(ind));

    % prints the whole solution, all node voltages and branch currents
%     qtr_async1.printSolution ();

    % extract data from the simulation
    namenodevoltage(ind,1) = qtr_async1.getnodev (wirelabel);
    
    vprobevoltage(ind,1) = qtr_async1.getvprobe (vprobename);

    iprobecurrent(ind,1) = qtr_async1.getiprobe (iprobename);

    iprobe2current(ind,1) = qtr_async1.getiprobe (iprobename2);

    subcktvprobevoltage(ind,1) = qtr_async1.getvprobe (subcktvprobe);

    % accept the step calculated previously into the solution history
    qtr_async1.acceptstep ();

end

% plot the voltages and currents extracted during simulation
figure; 
[AX, H1, H2] = plotyy ( t * 1000, ...
                        [namenodevoltage, vprobevoltage, subcktvprobevoltage], ...
                        t * 1000, ...
                        [iprobecurrent, iprobe2current] );

legend([H1;H2], wirelabel, vprobename, subcktvprobe, iprobename, iprobename2);

title ('Example Probe and Subcircuit Access', 'FontSize', 14)
xlabel('Time (ms)', 'FontSize', 14)
set(get(AX(1),'Ylabel'),'String','Voltages [V]', 'FontSize', 14) 
set(get(AX(2),'Ylabel'),'String','Current [A]', 'FontSize', 14) 

