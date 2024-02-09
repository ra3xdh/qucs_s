# Qucs-S: Quite universal circuit simulator with SPICE

[![Boosty](https://img.shields.io/badge/Boosty-donate-orange.svg)](https://boosty.to/qucs_s)
[![Telegram](https://img.shields.io/badge/Telegram-chat-blue.svg)](https://t.me/qucs_s)
[![Website](https://img.shields.io/badge/Website-ra3xdh.github.io-29d682.svg)](https://ra3xdh.github.io/)

## About Qucs-S

Qucs-S provides a fancy graphical user interface for a number of popular circuit simulation
engines. Qucs-S contains instruments for schematic capture, visualization and provides differents 
passive and active components including device library. The following simulation kernels are supported:

* Ngspice (recommended)
* Xyce
* SpiceOpus
* Qucsator (non-spice)


See the https://ra3xdh.github.io/ for more details. Qucs-S is based
on original Qucs code: https://github.com/Qucs/qucs

## Donation

Qucs-S accepts donation using Boosty platform: https://boosty.to/qucs_s

## Build instructions

Use CMake to build Qucs-S. Install all necessary dependencies: GCC, Qt, Flex, Bison and SPICE
(optional). Install `ngspice` that is not needed for build, but serves as the simulation kernel.

### Dependencies

#### Ubuntu

~~~
sudo apt-get install ngspice build-essential git cmake qtbase5-dev qttools5-dev libqt5svg5-dev flex bison
~~~

#### OpenSUSE Tumbleweed

~~~
sudo zypper install ngspice git cmake libqt5-qtbase-devel libqt5-qttools-devel libqt5-qtsvg-devel flex bison
~~~

### Compiling

#### Qt5

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

#### Qt6

Since v1.0.1 Qucs-S supports build with Qt6. Set the `QT_DIR` environment variable 
to tell CMake use the Qt6. For example use the following command sequence for Ubuntu-22.04

~~~
export QT_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt6
cmake ..  -DCMAKE_INSTALL_PREFIX=/your_install_prefix/
~~~ 

### Running

Then run `qucs-s` executable to launch the application:
~~~
cd /your_installation prefix/bin
./qucs-s
~~~

### clangd LSP support
Clangd looks for `compile_commands.json` file in parent folders of the file it processes.
`compile_commands.json` should be generated along with other build configuration files
when you run `cmake` as part of building routine:
```
mkdir builddir
cd builddir
cmake ..  -DCMAKE_INSTALL_PREFIX=/your_install_prefix/
```
If `compile_commands.json` is already there, create a symbolic link to it from project root dir:
```
cd project_root
ln -s ./builddir/compile_commands.json compile_commands.json
```

It may take some time to index files at first run. Clangd configuration is in `.clangd` file.
