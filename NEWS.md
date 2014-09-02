Qucs MASTER Changelog
=========================

Please update this file with the main changes. I will become the next release notes.
It will make our life easier to keep track of what have been done and to decide when it is time for a new release.


Miscelaneous
------------

 * new: top VERSION files shall be used for versioning (Autotools, CMake). TODO, package versioning.

 * new: add tag with the latest Git commit hash: Qucs (About and -v), Qucsator, Qucsconv


Bugfixes
--------

 * [#29]: https://github.com/Qucs/qucs/pull/29


---

Qucs 0.0.18 Release Notes
=========================

This release brings improvements on many parts of Qucs.
The port from Qt3 to Qt4 is still ongoing and any bugs and usability issues have been fixed in the process.
A few new features were added and others are in development.
See below a short summary of the most noticeable enhancements.

Qucs, Qucsator
--------------

 * new: Dynamic compilation and loading of Verilog-A modules (beta feature).
   Addition of a full ADMS/Qucs "turn key" Verilog-A compact device modelling system to
   Qucs. Users are no longer required to manually edit C++ code and the build system
   to be able to run Verilog-A models. Uses ADMS 2.3.4 (only a subset of Verilog-A is supported).

 * fix: Spice file handling and translator

 * new: Addition of BSIM 3 and BSIM 4 models to Qucs MOS models.

 * new: Release of the first model (RFresistor) in a new series of RF models for Harmonic Balance
   and other forms of Qucs simulation.

 * new: HICUM npn and pnp Level 0 V1.3 Verilog-A devices.

 * new: HICUM npn level 2 V2.32 Verilog-A device.


Qucs
----

 * new/fix : Ported several classes from Qt3Support to Qt4 (work in progress)

 * new: use QSettings to store user preferences and application settings

 * new: Copy/duplicate project content files

 * new: Export schematic and diagrams to SVG, PNG, JPG, JPEG, PDF, PDF+Latex, EPS (Inkscape required)

 * new: Component library is not included on the Dock Window

 * new: Added checkbox to settings, which allows to load schematics from future versions

 * new: Open Recent menu option

 * new: Set ADMS, ASCO and Octave paths on the application settings

 * new: Use environment variables ADMSXMLBINDIR, ASCOBINDIR, OCTAVEBINDIR to set tools paths

 * new: Updated translations: pt-BR, pt-PT, ru


Qucsator
--------

 * new: Refactoring of Qucsator namespace. Most of the core functions are under the namespace `qucs::`

 * new: Allow to run autotest by adding bugon/assert operator

 * new: initial support for unit testing with googletest.

 * new: added a Python scripts for parsing Qucs data files

 * fix: enable BJT models with (Vtf < 0)


QucsFilter, QucsAttenuator, QucsHelp, QucsEdit
----------------------------------------------

 * new/fix : Ported several classes from Qt3Support to Qt4 (work in progress)


QucsResCodes
------------

 * new: QucsResCodes is a program to compute color codes for resistors and resistance values from
   color codes. The program computes the closest standard resistor value. Users can paste the computed
   resistor in the schematic.


Interfaces
----------

 * Improved Qucs/Octave interface for GUI and Qucssator netlist entry (including a start
   on a Octave library of plotting functions for Qucs post simulation data visualisation)

 * First steps in developing a combined Qucs/Python Qucs simulation capability with
   high quality date visualisation using matplolib. Applications include basic sensitivity analysis
   and circuit performance optimisation.

 * Preliminary version of Qucs/Octave linked transient simulation capability.


Build system
------------

 * Add ADMS as a Git submodule to the Qucs Git repository

 * Added --disable-asco and --disable-adms option to skip the build of included sources

 * Added CMake (experimental) for advanced users building from Git repository

 * OSX: options to build against 10.5 to 10.9 SDK


Included packages
-----------------

 * ASCO 0.4.9 (patched)

 * ADMS 2.3.4


Miscelaneous
------------

 * Project website is now under version control.

 * Source code is now under Travis continuous integration service.

 * Source code documentation (Doxygen) available on the website.

 * Preliminary regression and test suite (qucs-test)


Bug fixes
---------

 * Hundreds of bug fixes and code improvements.



