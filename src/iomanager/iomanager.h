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

#ifndef IOMANAGER_H
#define IOMANAGER_H

#include <string>

using namespace std;

enum class Color
{
	black,
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan,
	white
};

class IOManager
{
public:
	virtual ~IOManager();

	// width of the terminal screen
	virtual int width() = 0;
	// height of the terminal screen
	virtual int height() = 0;

	// turn the cursor on or off
	virtual int cursor( bool val ) = 0;

	// replace the character at the current cursor location
	virtual void addChar( char c, Color col = Color::white ) = 0;
	// write a string of characters
	virtual void addString( string s, Color col = Color::white ) = 0;

	// move the cursor to the given coordinates
	virtual void moveCursor( int x, int y ) = 0;

	// combined functions
	virtual void moveAddChar( int x, int y, char c, Color col = Color::white );
	virtual void moveAddString( int x, int y, string s,
								Color col = Color::white );

	// read one character from the keyboard
	virtual char getChar() = 0;

	// show all changes made to the streambuffer
	virtual void refreshScreen() = 0;
};

#endif
