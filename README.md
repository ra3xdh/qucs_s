## Qucs-S: Quite universal circuit simulator with SPICE

Qucs-S provides a fancy graphical user interface for a number of popular circuit simulation
engines. Qucs-S contains libraries for schematic capture, visualization and components. The following
simulation kernels are supported:

* Ngspice (recommended)
* Xyce
* SpiceOpus
* Qucsator (non-spice)


See the https://ra3xdh.github.io/ for more details. Qucs-S is based
on original Qucs code: https://github.com/Qucs/qucs

## Donation

Qucs-S accepts donation using Boosty platform: https://boosty.to/qucs_s

## Build instructions

Use CMake to build Qucs-S. Install all necessary dependencies: GCC, Qt, and SPICE
(optional). Install `ngspice` that is not needed for build, but serves as the simulation kernel.

### Dependencies

#### Ubuntu

~~~
sudo apt-get install ngspice build-essential git cmake qtbase5-dev qttools5-dev libqt5svg5-dev
~~~

#### OpenSUSE Tumbleweed

~~~
sudo zypper install ngspice git cmake libqt5-qtbase-devel libqt5-qttools-devel libqt5-qtsvg-devel
~~~

### Compiling

Then clone this git repository and execute in the top directory:

~~~
mkdir builddir
cd builddir
cmake ..  -DCMAKE_INSTALL_PREFIX=/your_install_prefix/
make
make install
~~~

Where `/your_install_prefix/` is desired installation directory. Substitute any
desire path (for example `$HOME/qucs-s`) here. You may omit this option and
installation steps. Default installation directory will be `/usr/local` if
`CMAKE_INSTALL_PREFIX` is not defined.

### Running

Then run `qucs-s` executable to launch the application:
~~~
cd /your_installation prefix/bin
./qucs-s
~~~


