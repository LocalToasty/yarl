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

#ifndef CURSESTERMINAL_H
#define CURSESTERMINAL_H

#include "terminal.h"

class CursesTerminal : public Terminal
{
private:
	short cp(Color col);

public:
	CursesTerminal(bool usecolor = true);
	~CursesTerminal();

	int width();
	int height();

	int cursor(bool val);

	void addChar(char c, Color col = Color::white, bool standout = false);
	void addString(string s, Color col = Color::white, bool standout = false);

	void moveCursor(int x, int y);

	char getChar();

	void refreshScreen();
	void close();
};

#endif
