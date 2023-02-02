# Qucs-S 1.0.1

## New features

* Added support for Verilog-A defined devices using Ngspice+OpenVAF #197. Old Qucsator+ADMS
  workflow is marked as deprecated. Ngspice >=39 is required to support OpenVAF.
* Implemented variable parameter sweep analysis #189
* Added possibility to build Qucs-S with Qt6 #190 The -DWITH=QT6=ON option for cmake is required. 
  The source code of the Qucs-S is Qt6 compatible now. Thanks @Zergud !
* Simulator prefixes like "ngspice/" are not show anymore on plot traces if the plot contains data 
  from only one simulator #178 


## Component library

* Added TL071/TL072 opamp models
* Added 555_timer.lib library containing the 555 timer model
* Added extended BJT exteneded and Schottky diodes libraries

## Bugfix and general improvements

* Change default RCL device values and simulation settings to prevent convergence error when putting 
  together device with default values #195
* Fixed auto-scaling issue on Windows 64-bit platform #133

# Qucs-S 1.0.0

## New features

* Added basic support for logic gates simulation (Ngspice >= 38) required #97
* Added possibility to set engineering notation (like 1k, 1M, etc.) for numbers on diagrams #92
* Added symbol preview and search feature for component library in main windows #135
* Backported qucs-attenuator, qucs-transcalc and qucs-powercombining from Qucs-0.0.20 #142
* Added possibility to set dB units for Y-axis on digrams from the diagram properties dialog #92
* Backported advanced features for projects tree from Qucs-0.0.20 #95
* Show warning if schematic file name changed, but dataset/display file name are not updated #111
* Revised examples tree (by Tom Hajjar @tomhajjar); legacy Ngspice-incompatible examples 
  moved to `examples/qucsator` subdirectory

## Component library

* Qucs-lib tool has been removed from the application; all its features are
  availbale now from the main window left panel #139
* Fixed Tubes.lib and LEDs.lib libraries (by @tomhajjar)

## General improvements

* Fixed numerous bug reports. Thanks @tomhajjar and @Zergud for testing.

## Windows

* Added 32-bit Windows portable package

## Known issues

* Parameter sweep doesn't work when using the Fourier simulation with XYCE backend #73
* Netlister may produce duplicate .SUBCKT entry when using the same component from user and system library #82
* Auto-scaling on diagram may fail on some conditions for Windows 64-bit platform #133 Other platforms are not affected
* The 1-point AC simulation produce a set of 1x1 plot if ParameterSweep instead of smooth curve is attached #156


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

