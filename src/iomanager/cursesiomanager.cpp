/*
 * YARL - Yet another Roguelike
 * Copyright (C) 2015  Marko van Treeck <markovantreeck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cursesiomanager.h"
#include <curses.h>

short CursesIOManager::cp(Color col)
{
	switch (col)
	{
	case Color::black:
		return COLOR_BLACK;

	case Color::red:
		return COLOR_RED;

	case Color::green:
		return COLOR_GREEN;

	case Color::yellow:
		return COLOR_YELLOW;

	case Color::blue:
		return COLOR_BLUE;

	case Color::magenta:
		return COLOR_MAGENTA;

	case Color::cyan:
		return COLOR_CYAN;

	case Color::white:
		return COLOR_WHITE;
	}
}

CursesIOManager::CursesIOManager( bool usecolor, bool cursor )
{
	initscr();
	cbreak();
	noecho();
    curs_set( cursor );	// no cursor

	// initialize colors
	if ( has_colors() && usecolor )
	{
		start_color();

		for ( int i = 1; i < 8; i++ )
			init_pair( i, i, COLOR_BLACK );
	}
}

CursesIOManager::~CursesIOManager()
{
	endwin();
}

int CursesIOManager::width()
{
	return getmaxx(stdscr);
}

int CursesIOManager::height()
{
	return getmaxy( stdscr );
}

int CursesIOManager::cursor( bool val)
{
	curs_set(val);
}

void CursesIOManager::addChar( char c, Color col )
{
	attrset( COLOR_PAIR( cp( col ) ) );
	addch( c );
}

void CursesIOManager::addString( string s, Color col)
{
	attrset( COLOR_PAIR( cp( col ) ) );
	addstr(s.c_str());
}

void CursesIOManager::moveCursor( int x, int y )
{
	move( y, x );
}

char CursesIOManager::getChar()
{
    return ( char ) getch();
}

void CursesIOManager::clear( int x, int y, int w, int h )
{
    for( int i = 0; i < w; i++ )
    {
        move( x, y );
        for( int j = 0; j < h; h++ )
        {
            addch( ' ' );
        }
    }
}

void CursesIOManager::refreshScreen()
{
	refresh();
}
