% asynchronous_boost_converter_example.m
%
% 

cd(fileparts(which('asynchronous_boost_converter_example.m')));

tstart = 0;
n = 100;
tend = 5e-4;

%% fixed-step synchonous solver test

clear qtr_async1

% create a new asynchronous solver object from a netlist
qtr_async1 = asynctrcircuit('boostconverter.net');

% initialist the simulation
qtr_async1.init(tstart, (tend - tstart) / (10 * n));

% get the number of nodes
N = qtr_async1.getn;

% get the number of voltage sources
M = qtr_async1.getm;

% choose some time points
T1 = linspace(tstart, tend, n)';
% initialise storage for the solution
Y1 = zeros(numel(T1), M+N);
% get the initial solution
Y1(1, 1:(N+M)) = qtr_async1.getsolution();
% step through time solving the circuit
for ind = 2:numel(T1)
    
    % accept the step into the solution history
	qtr_async1.acceptstep(T1(ind));
    
    % get the node voltages and currents at the current time
    Y1(ind, 1:(N+M)) = qtr_async1.getsolution();

end

% plot the node voltages and branch currents
figure; 
AX = plotyy(T1 * 1000, Y1(:,1:N), T1 * 1000, Y1(:,((N+1):(N+M))));
title ('Boost Converter', 'FontSize', 14)
xlabel('Time (ms)', 'FontSize', 14)
set(get(AX(1),'Ylabel'),'String','Node Voltages', 'FontSize', 14) 
set(get(AX(2),'Ylabel'),'String','Branch Currents', 'FontSize', 14) 