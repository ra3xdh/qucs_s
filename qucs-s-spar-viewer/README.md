# Qucs-S S-Parameter Viewer

A Qt-based application for visualizing S-parameter data.

## Features

### Data Import
- **Touchstone File Support**: Read standard .snp files (S1P, S2P, S3P, etc.)
- **Qucs Dataset Support**: Import data from both Qucsator and NGspice formats

### Visualization
- **Multiple Plot Types**:
  - Rectangular plots (Magnitude/Phase, Impedance, Stability, VSWR, Group Delay)
  - Smith Chart (Z and Y chart options)
  - Polar plots
- **Trace Management**: Add, remove, and customize traces with color, width, and line style
- **Markers**: Place frequency markers with automatic value readout
- **Limit Lines**: Define pass/fail criteria with frequency-dependent limits

### Session Management
- **Save/Load Sessions**: Preserve complete workspace including:
  - All loaded datasets
  - Plot configurations
  - Trace properties
  - Markers and limits
  - Notes

### Synthesis Tools
- **Filter Design**: Canonical and Direct-Coupled LC filters. Transmission line filters
- **Matching Newtwork Design**: L-section, Stub-matching, impedance transformers
- **Power Combining Design**: Various power combiner and coupler topologies including Wilkinson, Bagley, Gysel, Lim-Eom, etc.
- **Attenuator Design**: Pi, Tee, Bridged-Tee and tuned attenuators, including power dissipation calculations.
- **Netlist Scratchpad**: Text-based tool to simulate the S-parameters of a netlist as you type

- All the tools can export the synthesized circuits to Qucs-S via clipboard.

## Building

### Requirements
- Qt 6.x
- C++11 or later compiler
- CMake

### Dependencies
- Qt Widgets
- Qt XML

### Compilation
```bash
mkdir build && cd build
cmake ..
make
sudo make install
```

## Usage

### Loading Data
1. **File → Open** to load Touchstone or Qucs dataset files
2. Select parameters from the matrix-style parameter selector
3. Choose display mode (Magnitude/Phase, Smith, Polar, etc.)
4. Click **Add Trace** to visualize

### Adding Markers
1. Navigate to the **Markers** tab
2. Enter frequency and scale
3. Click **Add Marker**
4. View values in the marker table

### Setting Limits
1. Go to **Limits** tab
2. Define frequency range and value thresholds
3. Limits appear as shaded regions on plots

### Saving Work
- **File → Save Session** to preserve entire workspace
- Session files (.spar) store all data and settings
- **File → Recent Files** for quick access

## File Formats

### Touchstone (.snp)
Standard format for N-port S-parameter data. Supports:
- Frequency sweeps
- MA (Magnitude-Angle), RI (Real-Imaginary), DB (dB-Angle) formats
- Reference impedance specification

### Qucs-S Dataset
Qucs-S format supporting:
- **Qucsator format**: `S[i,j]` matrix notation
- **NGspice format**: `ac.v(s_j_i)` voltage source notation
- Multiple parameter types (S, Y, Z)

### Session Files (.spar)
XML-based format storing complete application state.

## License

GNU General Public License v3.0 or later

## Author

Andrés Martínez Mera - andresmmera@protonmail.com

## Copyright

Copyright (C) 2026 Andrés Martínez Mera
