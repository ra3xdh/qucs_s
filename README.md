[![LiberaPay](https://liberapay.com/assets/widgets/donate.svg)](https://liberapay.com/ra3xdh/donate) ![LiberaPay](https://img.shields.io/liberapay/patrons/ra3xdh.svg?logo=liberapay)

## Qucs-S: Quite universal circuit simulator with SPICE

Qucs-S provides a fancy graphical user interfyce for a number of popular circuit simulation 
engines. Qucs-S contains schematic capture, visualization and component library. The following
simulation kernels are supported:

* Ngspice (recommended)
* Xyce 
* SpiceOpus
* Qucsator (non-spice)


See the https://ra3xdh.github.io/ for more details. Qucs-S is based
on original Qucs code: https://github.com/Qucs/qucs

## Build instructions

Use CMake to build Qucs-S. Install all necessary dependecies: GCC, Qt, and SPICE
(optional). For Ubuntu launch the following command to install developement tools:

~~~
sudo apt-get install build-essential git cmake qtbase5-dev qttools5-dev qtscript5-dev libqt5svg5-dev
~~~

Install `ngspice` that is not needed for build but serves as the simulation kernel.

~~~
sudo apt-get install ngspice
~~~

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

Then run `qucs-s` executable to launch the application:
~~~
cd /your_installation prefix/bin
./qucs-s
~~~


