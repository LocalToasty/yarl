# YARL - Yet another Roguelike
Copyright &copy; 2015-2016 Marko van Treeck
&lt;<markovantreeck@gmail.com>&gt;

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but
**without any warranty**; without even the implied warranty of
**merchantability** or **fitness for a particular purpose**. Refer to
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see &lt;<http://www.gnu.org/licenses/>&gt;.


## What is YARL?

YARL is *yet another roguelike* - a role playing game with a randomly
generated and highly dynamic world.

Being in its early development stage, YARL does not yet have all its
features implemented. Therefore the current stage of the project does
not represent the final product.


## How to compile

YARL can be compiled using [CMake](http://www.cmake.org/) 3.1 or later:

`$ cmake . && make`

CMake can be invoked with an command line option to specify whether or
not SDL should be used as the I/O backend:

`$ cmake -DUSE_SDL=<ON|OFF> .`

If USE_SDL is turned off, a Curses implementation will be used.

Should cmake fail to find your SDL libraries you can specify their
location:

`$ cmake -DUSE_SDL=ON -DSDL2_INCLUDE_DIR=<include dir>
-DSDL2_LIBRARY=<library dir>/SDL2.lib -DSDL2MAIN_LIBRARY=<library
dir>/SDL2main.lib .`
