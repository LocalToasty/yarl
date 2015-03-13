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

#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>

using namespace std;

class Terminal
{
public:
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

	virtual int Renderer() = 0;

	virtual int width() = 0;
	virtual int height() = 0;

	virtual void addch(char c, Color c = Color::white, bool dim = false) = 0;
	virtual void addstr(string s, Color c = Color::white, bool dim = false) = 0;
	virtual void move(int x, int y) = 0;

	virtual char getch() = 0;
};

#endif
