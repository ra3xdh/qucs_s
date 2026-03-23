import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import re
import sys

# ─── SIMULATOR SELECTION ─────────────────────────────────────────────────────
# Set to 'qucsator' or 'ngspice'
SIMULATOR = sys.argv[1] if len(sys.argv) > 1 else 'qucsator'

if SIMULATOR == 'qucsator':
    DAT_FILE = 'LPF.dat'
    KEY_FREQ = 'frequency'
    KEY_S11  = 'S[1,1]'
    KEY_S21  = 'S[2,1]'
elif SIMULATOR == 'ngspice':
    DAT_FILE = 'LPF.dat.ngspice'
    KEY_FREQ = 'frequency'
    KEY_S11  = 'ac.v(s_1_1)'
    KEY_S21  = 'ac.v(s_2_1)'
else:
    raise ValueError(f"Unknown SIMULATOR: {SIMULATOR!r}. Use 'qucsator' or 'ngspice'.")



# ─── LOAD QUCS-S DATA ────────────────────────────────────────────────────────

def parse_qucs_dat(filename):
    """Parse a Qucs dataset file and return a dict of {variable: np.array}."""
    data = {}
    current_var = None
    values = []
    indep_vars = {}

    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()

            # Independent variable (e.g. frequency)
            m = re.match(r'<indep\s+(\S+)\s+\d+>', line)
            if m:
                current_var = m.group(1)
                values = []
                continue

            # Dependent variable (e.g. S[1,1])
            m = re.match(r'<dep\s+(\S+)\s+\S+>', line)
            if m:
                current_var = m.group(1)
                values = []
                continue

            # End of block
            if line.startswith('</'):
                if current_var is not None:
                    data[current_var] = np.array(values)
                current_var = None
                values = []
                continue

            # Data line — may be real or complex (Qucs uses 'j' notation: a-jb)
            if current_var is not None and line:
                line_py = re.sub(r'([+-])j([0-9eE.+\-]+)', r'\1\2j', line)
                try:
                    values.append(complex(line_py))
                except ValueError:
                    values.append(float(line_py))

    return data

qucs_data = parse_qucs_dat(DAT_FILE)

qucs_freq_Hz = np.real(qucs_data[KEY_FREQ])
qucs_freq_MHz = qucs_freq_Hz / 1e6
qucs_s11 = qucs_data[KEY_S11]
qucs_s21 = qucs_data[KEY_S21]
qucs_s11_db = 20 * np.log10(np.abs(qucs_s11))
qucs_s21_db = 20 * np.log10(np.abs(qucs_s21))

# ─── PLOT SETTINGS ───────────────────────────────────────────────────────────

plt.rcParams.update({
    'axes.facecolor':   'white',
    'figure.facecolor': 'white',
    'axes.edgecolor':   'black',
    'axes.labelcolor':  'black',
    'xtick.color':      'black',
    'ytick.color':      'black',
    'text.color':       'black',
    'grid.color':       '#cccccc',
    'legend.facecolor': 'white',
    'legend.edgecolor': 'black',
    'figure.dpi': 80,
})

plt.close('all')

freq_markers = [600, 1000]  # MHz
vert_markers = [600, 1000]  # MHz

fig, ax = plt.subplots(figsize=(11, 6))

# Qucs-S traces (dashed)
ax.plot(qucs_freq_MHz, qucs_s11_db, lw=1.5, label='S11',    color='blue',  ls='-')
ax.plot(qucs_freq_MHz, qucs_s21_db, lw=1.5, label='S21',    color='red',   ls='-')

# ─── Y-AXIS: 5 dB/div ────────────────────────────────────────────────────────
ax.set_ylim(-40, 5)
ax.yaxis.set_major_locator(ticker.MultipleLocator(5))
ax.yaxis.set_minor_locator(ticker.MultipleLocator(1))

# ─── VERTICAL MARKERS ────────────────────────────────────────────────────────
for vf in vert_markers:
    ax.axvline(vf, color='green', lw=1.2, ls='--', alpha=0.85, zorder=3)
    ax.text(vf, 0.5, f'{vf} MHz',
            transform=ax.get_xaxis_transform(),
            color='green', fontsize=8, ha='center', va='bottom',
            bbox=dict(boxstyle='round,pad=0.2', fc='white', ec='green', alpha=0.85))

# ─── FREQUENCY MARKERS (scikit-rf only) ──────────────────────────────────────
traces = [
    (qucs_s11_db, qucs_freq_MHz, 'S11', 'blue', 'D'),
    (qucs_s21_db, qucs_freq_MHz, 'S21', 'red',  'D'),
]

for i, mf_MHz in enumerate(freq_markers):
    for trace_db, trace_freq, name, color, mkr in traces:
        val = float(np.interp(mf_MHz, trace_freq, trace_db))

        if not (-40 <= val <= 0):
            continue

        ax.plot(mf_MHz, val, marker=mkr, ms=8, color=color,
                markeredgecolor='black', markeredgewidth=0.7, zorder=6)

        y_offset = 12 if i % 2 == 0 else -32

        ax.annotate(
            f'Mk{i+1} {name}\n{mf_MHz:.0f} MHz\n{val:.1f} dB',
            xy=(mf_MHz, val),
            xytext=(6, y_offset),
            textcoords='offset points',
            fontsize=7.5,
            color=color,
            bbox=dict(boxstyle='round,pad=0.3', fc='white', ec=color, alpha=0.9),
            arrowprops=dict(arrowstyle='->', color=color, lw=0.8),
        )

# ─── FORMATTING ──────────────────────────────────────────────────────────────
ax.set_xlabel('Frequency (MHz)', fontsize=12)
ax.set_ylabel('Magnitude (dB)',  fontsize=12)
ax.set_title(f'S-Parameters', fontsize=12)
ax.legend(fontsize=10)
ax.grid(True, which='major', lw=0.7, alpha=0.7)
ax.grid(True, which='minor', lw=0.3, alpha=0.4, ls=':')
ax.set_xlim(qucs_freq_MHz[0], qucs_freq_MHz[-1])
fig.subplots_adjust(left=0.08, right=0.97, top=0.93, bottom=0.10)

plt.show(block=True)
