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

#ifndef SDLIOManager_H
#define SDLIOManager_H

#include "iomanager.h"
#include <SDL2/SDL.h>
#include <vector>

using namespace std;

class SDLIOManager : public IOManager
{
private:
	static const int _defaultWidth;
	static const int _defaultHeight;

	SDL_Window* _window;
	SDL_Surface* _charset;

	vector<char> _characters;
	vector<Color> _colors;

	// character dimensions
	int _charWidth;
	int _charHeight;

	bool _cursorOn;

	// screen dimensions (in characters)
	int _width { _defaultWidth };
	int _height { _defaultHeight };

	// cursor position
	int _cursX { 0 };
	int _cursY { 0 };

	bool _useColor;

	Uint32 color( Color col );

public:
	SDLIOManager( bool usecolor, bool cursor = false,
				  const char* charset = "charset.bmp" );
	~SDLIOManager();

	int width();
	int height();

	void cursor( bool val );
	int cursorX();
	int cursorY();

	void addChar( char c, Color col = Color::white );
	void addString( string s, Color col = Color::white );

	void moveCursor( int x, int y );

	char getChar();

	void clear( int x = 0, int y = 0, int w = -1, int h = -1 );

	void refreshScreen();
};

#endif
