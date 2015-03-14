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

#include "cursesterminal.h"
#include <curses.h>

short CursesTerminal::cp(Color col)
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

CursesTerminal::CursesTerminal(bool usecolor)
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);	// no cursor

	// initialize colors
	if (has_colors() && usecolor)
	{
		start_color();
		for (int i = 1; i < 8; i++)
			init_pair(i, i, COLOR_BLACK);
	}
}

int CursesTerminal::width()
{
	return getmaxx(stdscr);
}

int CursesTerminal::height()
{
	return getmaxy(stdscr);
}

int CursesTerminal::cursor(bool val)
{
	curs_set(val);
}

void CursesTerminal::addChar(char c, Color col, bool standout)
{
	attrset(COLOR_PAIR(cp(col)) | (standout ? A_BOLD : A_NORMAL));
	addch(c);
}

void CursesTerminal::addString(string s, Color col, bool standout)
{
	attrset(COLOR_PAIR(cp(col)) | (standout ? A_BOLD : A_NORMAL));
	addstr(s.c_str());
}

void CursesTerminal::moveCursor(int x, int y)
{
	move(y, x);
}

char CursesTerminal::getChar()
{
	return (char) getch();
}

void CursesTerminal::refreshScreen()
{
	refresh();
}

void CursesTerminal::close()
{
	endwin();
}
