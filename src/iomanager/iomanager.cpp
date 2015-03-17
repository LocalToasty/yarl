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

#include "iomanager.h"

IOManager::~IOManager()
{
}

void IOManager::moveAddChar( int x, int y, char c, Color col )
{
	moveCursor( x, y );
	addChar( c, col );
}


void IOManager::moveAddString( int x, int y, std::string s, Color col )
{
	moveCursor( x, y );
	addString( s, col );
}
