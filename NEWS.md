# Qucs-S 0.0.24

## New simulation types

* Added S-parameter simulation using Ngspice and Xyce backends; Ngspice>=37 requiered #84, #79
* Added FFT analysis with Ngspice backend #88

## GUI improvements

* Implemented dark desktop theme support #66
* Implemented manual setting of the X-axis markers postion #92
* Added separate setting for schematic and application font

## Qt5 support

* Removed QtScript dependency (thanks @Zergud) #77
* Fixed Qt5.15 deprecation warnings #75

## Component libraries

* Added quartz crystal library (Crystal.lib); the quartz model is optimized for filter design;
* Added TL431 device in Regulators library;

## General bugfixes

* Fixed crash on HB simulation with XYCE #28
* Fixed empty path for working directory #86
* Show error when schematic contains only DC simulation (no effect for Ngspice) #83
* Fixed layout in arrow properties dialog #81
* Fixed drag'n'drop operation from left dock to schematic
* Fixed newlines in SPICE resitors #62
* Fixed crash on Wayland #76

## Documentation and examples

* Added offline PDF tutorial; the link for old Qucs tutorial removed from Help menu #78
* Added examples for new S-parameter and FFT simulations
* Fixed distortion simulation example #83

## Known issues

* Parameter sweep doesn't work when using the Fourier simulation with XYCE backend #73
* Netlister may produce duplicate .SUBCKT entry when using the same component from user and system library #82

# Qucs-S 0.0.23

## Qt5 support and new features

* The Qucs-S application is now fully ported to Qt5 and could be compiled on modern Linux distributions; #72 #32
* Added two new component libraries: BF998 and Tubes containing two-gate MOSFET and vacuum tubes (triodes and penthodes provided by @olegkapitonov ) models;
* Windows binary switched to 64-bit build. The old 32-bit binaries are not provided anymore;
* Ngspice is now the default simulation kernel on the first application start;
* The Qucs-S doesn't use a shared settings file with Qucs anymore #71

## General bugfixes

* Fixed "Show last netlists" not showing SPICE netlist when SPICE engine is selected #41
* Use system default sans serif font instead of "Helvetica", because "Helvetica" cannot be rendered correctly on some platforms;
* Fixed .INCLUDE directive support with SpiceOpus #16
* Partially fixed .FOUR simulation support for Xyce backend #73
* Removed warning about experimental feature in the simulation dialog. Qucs-S is considered to be stable now;

