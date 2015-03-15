# YARL - Yet another Roguelike
Copyright &copy; 2015  Marko van Treeck &lt;<markovantreeck@gmail.com>&gt;

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but **without any warranty**; without even the implied warranty of
**merchantability** or **fitness for a particular purpose**.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see &lt;<http://www.gnu.org/licenses/>&gt;.


## What is YARL?

YARL is *yet another roguelike* - a role playing game with a randomly
generated and highly dynamic world.

Being in its early development stage, YARL does not yet have all its
features implemented. Therefore the current stage of the project
does not represent the final product.


## How to compile

YARL can be compiled using [CMake](http://www.cmake.org/) 3.1 or later:

`$ cmake . && make`

Due to CMake 3.1 being relatively new, many distributions do not yet offer
it in their repositories. On such systems YARL can be compiled with the
following command:

``$ g++ --std=c++11 `pkg-config --libs --cflags ncurses` -o yarl src/*.cpp``
