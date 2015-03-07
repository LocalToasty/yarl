#ifndef GLOBALS_H
#define GLOBALS_H

#include <curses.h>

enum ColorFlags
{
		fg_black	= 0,
		fg_red		= 1,
		fg_green	= 2,
		fg_yellow	= 3,
		fg_blue		= 4,
		fg_magenta	= 5,
		fg_cyan		= 6,
		fg_white	= 7,

		bg_black	= 0 << 3,
		bg_red		= 1 << 3,
		bg_green	= 2 << 3,
		bg_yellow	= 3 << 3,
		bg_blue		= 4 << 3,
		bg_magenta	= 5 << 3,
		bg_cyan		= 6 << 3,
		bg_white	= 7 << 3
};

enum class Command
{
	north,
	south,
	west,
	east,
	northWest,
	northEast,
	southWest,
	southEast,
	quit
};

#endif
