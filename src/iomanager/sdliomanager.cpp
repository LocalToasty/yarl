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

#include "sdliomanager.h"
#include <SDL2/SDL.h>
#include <iostream>

using namespace std;

const int SDLIOManager::_defaultWidth = 80;
const int SDLIOManager::_defaultHeight = 50;

Uint32 SDLIOManager::color(Color col)
{
	SDL_Surface* screen = SDL_GetWindowSurface( _window );

	switch( col )
	{
	case Color::black:
		return SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 );

	case Color::red:
		return SDL_MapRGB( screen->format, 0xff, 0x00, 0x00 );

	case Color::green:
		return SDL_MapRGB( screen->format, 0x00, 0xff, 0x00 );

	case Color::yellow:
		return SDL_MapRGB( screen->format, 0xff, 0xff, 0x00 );

	case Color::blue:
		return SDL_MapRGB( screen->format, 0x00, 0x00, 0xff );

	case Color::magenta:
		return SDL_MapRGB( screen->format, 0xff, 0xff, 0xff );

	case Color::cyan:
		return SDL_MapRGB( screen->format, 0xff, 0x00, 0x00 );

	case Color::white:
		return SDL_MapRGB( screen->format, 0xff, 0xff, 0xff );
	}
}

SDLIOManager::SDLIOManager( bool usecolor, bool cursor, const char* charset ) :
	_cursorOn( cursor )
{
	_characters = vector<char>( _width * _height, ' ' );
	_colors = vector<Color>( _width * _height, Color::white );

	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS ) < 0 )
	{
		cerr << "Error while initializing SDL: " << SDL_GetError() << endl;
		return; // TODO
	}

	SDL_Surface* tmpCharset = SDL_LoadBMP(charset);

	if ( tmpCharset == nullptr )
	{
		cerr << "Error while trying to open char set: "
			 << SDL_GetError() << endl;
		return; // TODO
	}

	_charWidth = tmpCharset->w / 16;
	_charHeight = tmpCharset->h / 8;

	_window = SDL_CreateWindow( "yarl",
								SDL_WINDOWPOS_UNDEFINED,
								SDL_WINDOWPOS_UNDEFINED,
								_defaultWidth * _charWidth,
								_defaultHeight * _charHeight,
								SDL_WINDOW_SHOWN );

	if ( _window == nullptr )
	{
		cerr << "Error while opening window: "
			 << SDL_GetError() << endl;
		return; // TODO
	}

	SDL_Surface* screen = SDL_GetWindowSurface( _window );

	// optimise charset format
	_charset = SDL_ConvertSurface( tmpCharset, screen->format, 0 );
	SDL_FreeSurface( tmpCharset );

	// set color key
	SDL_SetColorKey( _charset, SDL_TRUE,
					 SDL_MapRGB( screen->format, 0xff, 0xff, 0xff ) );

	// clear screen;
	SDL_FillRect( screen, nullptr, SDL_MapRGB( screen->format, 0, 0, 0 ) );
	SDL_UpdateWindowSurface( _window );
}

SDLIOManager::~SDLIOManager()
{
	SDL_DestroyWindow( _window );
	SDL_FreeSurface( _charset );

	SDL_Quit();
}

int SDLIOManager::width()
{
	return _width;
}

int SDLIOManager::height()
{
	return _height;
}

int SDLIOManager::cursor( bool val )
{
	_cursorOn = val;
}

void SDLIOManager::addChar( char c, Color col )
{	
	_characters.at( _cursX + _width * _cursY ) = c;
	_colors.at( _cursX + _width * _cursY ) = col;

	_cursX++;

	if ( _cursX > _width )
	{
		_cursX = 0;
		_cursY++;
	}
}

void SDLIOManager::addString( string s, Color col )
{
	for ( char c : s )
		addChar( c, col );
}

void SDLIOManager::moveCursor( int x, int y )
{
	if ( x >= _width )
		_cursX == _width;
	else if ( x < 0 )
		_cursX == 0;
	else
		_cursX = x;

	if ( y >= _height )
		_cursY == _height;
	else if ( y < 0 )
		_cursY == 0;
	else
		_cursY = y;
}

char SDLIOManager::getChar()
{
	SDL_Event e;
	for( ;; )	// getChar has to be blocking
	{
		while( SDL_PollEvent( &e ) )
		{
			if( e.type == SDL_QUIT )
			{
				return 0;
			}

			else if( e.type == SDL_KEYDOWN )
			{
				return ( char ) e.key.keysym.sym;
			}
		}

		SDL_Delay( 20 );	// to lower CPU usage
	}
}

void SDLIOManager::clear( int x, int y, int w, int h )
{
	for( int i = 0; i < w; i++ )
	{
		for( int j = 0; j < h; h++ )
		{
			_characters.at( x + i + ( y + j ) * _width ) = ' ';
		}
	}
}

void SDLIOManager::refreshScreen()
{
		SDL_Surface* screen = SDL_GetWindowSurface( _window );

		for( int y = 0; y < _height; y++ )
		{
			for( int x = 0; x < _width; x++ )
			{
				char c = _characters.at( x + y * _width );
				Color col = _colors.at( x + y * _width );

				SDL_Rect ch;
				ch.x = _charWidth * ( c & 0x0f );
				ch.y = _charHeight * ( ( c & 0x70 ) >> 4 );
				ch.w = _charWidth;
				ch.h = _charHeight;

				SDL_Rect pos;
				pos.x = _charWidth * x;
				pos.y = _charHeight * y;
				pos.w = _charWidth;
				pos.h = _charHeight;

				SDL_FillRect( screen, &pos, color(col) );
				SDL_BlitSurface (_charset, &ch, screen, &pos );
			}
		}

	SDL_UpdateWindowSurface( _window );
}
