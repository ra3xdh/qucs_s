## Description

Qucs-S is a spin-off of Qucs. It allows to use several SPICE and non-SPICE 
simulation kernels. See https://ra3xdh.github.io/ for more details. It is based
on original Qucs code: https://github.com/Qucs/qucs

## Build instructions

Use CMake to build Qucs-S. Install all necessary dependecies: GCC, Qt, and SPICE
(optional). Clone this git repository and execute in the top directory:

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

Then run `qucs-s` executable to launch application:
~~~
cd /your_installation prefix/bin
./qucs-s
~~~


