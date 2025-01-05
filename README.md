# Qucs-S: Quite universal circuit simulator with SPICE

[![Boosty](https://img.shields.io/badge/Boosty-donate-orange.svg)](https://boosty.to/qucs_s)
[![Telegram](https://img.shields.io/badge/Telegram-chat-blue.svg)](https://t.me/qucs_s)
[![Website](https://img.shields.io/badge/Website-ra3xdh.github.io-29d682.svg)](https://ra3xdh.github.io/)
[![Packaging status](https://repology.org/badge/tiny-repos/qucs-s.svg)](https://repology.org/project/qucs-s/versions)

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

Qucs-S requires Qt6 libraries including QtCharts, CMake, flex, bison, gperf, and dos2unix as compile time
dependencies. Install these packages using the package manager of your distribution before compiling Qucs-S.
Ngspice is not required at compile time, but it is required as runtime dependency to run the simulation.

Here are some examples for the popular Linux distributions.

#### Ubuntu or Debian

~~~
sudo apt-get install ngspice build-essential git cmake flex bison gperf dos2unix
sudo apt-get install qt6-base-dev qt6-tools-dev qt6-tools-dev-tools libglx-dev linguist-qt6 
sudo apt-get install qt6-l10n-tools libqt6svg6-dev libgl1-mesa-dev qt6-charts-dev libqt6opengl6-dev
~~~

#### Fedora

~~~
sudo dnf install gcc-c++ cmake git flex bison gperf dos2unix ngspice
sudo dnf install qt6-qtbase-devel cmake qt6-qtsvg-devel qt6-qttools-devel qt6-qtcharts-devel 
~~~

### Compiling

After installing the dependecies, clone this git repository and execute in the top directory:

~~~
git submodule init
git submodule update
mkdir builddir
cd builddir
cmake ..  -DCMAKE_INSTALL_PREFIX=/your_install_prefix/
make
make install
~~~

Since the v25.1.0 the Qucs-S will be configured with Qt6 by default. Substutute the  `/your_install_prefix/` 
as desired installation directory. Substitute any desire path (for example `$HOME/qucs-s`) here. 
You may omit this option and installation steps. Default installation directory will be `/usr/local` if
`CMAKE_INSTALL_PREFIX` is not defined.

### Qt5/Qt6 support

Qt5 support has been dropped since v25.1.0. Only Qt6 libraries are supported. Set the `WITH_QT6=ON` 
cmake flag if compiling the Qucs-S versions before v25.1.0


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
