#ifndef GLOBALS_H
#define GLOBALS_H

#include <curses.h>

enum Command
{
	north		= 'k',
	south		= 'j',
	west		= 'h',
	east		= 'l',
	northWest	= 'y',
	northEast	= 'u',
	southWest	= 'b',
	southEast	= 'n',
	quit		= 'q'
};

#endif
