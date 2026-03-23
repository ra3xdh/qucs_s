% OctavePlot.m - main script
% Requires parse_qucs_dat.m in the same directory.

% ─── SIMULATOR SELECTION ─────────────────────────────────────────────────────
% Set to 'qucsator' or 'ngspice'
args = argv();
SIMULATOR = 'qucsator';
if numel(args) > 0
  SIMULATOR = args{1};
end

if strcmp(SIMULATOR, 'qucsator')
  DAT_FILE = 'LPF.dat';
  KEY_FREQ = 'frequency';
  KEY_S11  = 'S[1,1]';
  KEY_S21  = 'S[2,1]';
elseif strcmp(SIMULATOR, 'ngspice')
  DAT_FILE = 'LPF.dat.ngspice';
  KEY_FREQ = 'frequency';
  KEY_S11  = 'ac.v(s_1_1)';
  KEY_S21  = 'ac.v(s_2_1)';
else
  error('Unknown SIMULATOR: %s. Use qucsator or ngspice.', SIMULATOR);
end

% ─── PARSE DATA ──────────────────────────────────────────────────────────────

qucs_data = parse_qucs_dat(DAT_FILE);

qucs_freq_Hz  = real(qucs_data(KEY_FREQ));
qucs_freq_MHz = qucs_freq_Hz / 1e6;
qucs_s11      = qucs_data(KEY_S11);
qucs_s21      = qucs_data(KEY_S21);
qucs_s11_db   = 20 * log10(abs(qucs_s11));
qucs_s21_db   = 20 * log10(abs(qucs_s21));

% ─── PLOT ────────────────────────────────────────────────────────────────────

close all;

freq_markers = [600, 1000];  % MHz

fig = figure('Position', [100, 100, 880, 480], 'Color', 'white');
ax  = axes('Parent', fig);

hold(ax, 'on');

% S-parameter traces
plot(ax, qucs_freq_MHz, qucs_s11_db, '-', 'LineWidth', 1.5, ...
     'Color', 'blue', 'DisplayName', 'S11');
plot(ax, qucs_freq_MHz, qucs_s21_db, '-', 'LineWidth', 1.5, ...
     'Color', 'red',  'DisplayName', 'S21');

% ─── Y-AXIS: 5 dB/div ────────────────────────────────────────────────────────
ylim(ax, [-40, 5]);
set(ax, 'YTick', -40:5:5);
y_lims = ylim(ax);

% ─── FREQUENCY MARKERS ───────────────────────────────────────────────────────
traces = { ...
  qucs_s11_db, qucs_freq_MHz, 'S11', [0 0 1]; ...
  qucs_s21_db, qucs_freq_MHz, 'S21', [1 0 0]  ...
};

for i = 1:numel(freq_markers)
  mf_MHz = freq_markers(i);
  for t = 1:size(traces, 1)
    trace_db   = traces{t, 1};
    trace_freq = traces{t, 2};
    name       = traces{t, 3};
    color      = traces{t, 4};

    val = interp1(trace_freq, trace_db, mf_MHz);

    if val < -40 || val > 0
      continue
    end

    plot(ax, mf_MHz, val, 'd', 'MarkerSize', 8, ...
         'MarkerFaceColor', color, 'MarkerEdgeColor', 'black', ...
         'LineWidth', 0.7, 'HandleVisibility', 'off');

    y_range = y_lims(2) - y_lims(1);
    if mod(i, 2) == 1
      y_data_off = y_range * 0.06;
      va = 'bottom';
    else
      y_data_off = -y_range * 0.10;
      va = 'top';
    end

    text(ax, mf_MHz + 10, val + y_data_off, ...
         sprintf('Mk%d %s\n%d MHz\n%.1f dB', i, name, mf_MHz, val), ...
         'Color', color, 'FontSize', 10, ...
         'VerticalAlignment', va, ...
         'BackgroundColor', 'white', 'EdgeColor', color);
  end
end

% ─── FORMATTING ──────────────────────────────────────────────────────────────
xlabel(ax, 'Frequency (MHz)', 'FontSize', 12);
ylabel(ax, 'Magnitude (dB)',  'FontSize', 12);
title(ax,  'S-Parameters',    'FontSize', 12);
legend(ax, {'S11', 'S21'}, 'FontSize', 10);
grid(ax, 'on');
grid(ax, 'minor');
xlim(ax, [qucs_freq_MHz(1), qucs_freq_MHz(end)]);

set(ax, 'Color', 'white', 'XColor', 'black', 'YColor', 'black', ...
        'GridColor', [0.4 0.4 0.4], 'GridAlpha', 1.0, ...
        'MinorGridColor', [0.6 0.6 0.6], 'MinorGridAlpha', 1.0, 'Box', 'on');

set(fig, 'Color', 'white');
hold(ax, 'off');
waitfor(fig);
