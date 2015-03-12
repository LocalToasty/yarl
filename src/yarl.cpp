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

#include "yarl.h"

#include "item.h"
#include "generator.h"
#include <curses.h>
#include <stdexcept>

using namespace std;

Tile none		= Tile(' ', COLOR_BLACK,	"void");
Tile ground		= Tile('.', COLOR_WHITE,	"a stone floor", true);
Tile wallNS		= Tile('|', COLOR_WHITE,	"a stone wall");
Tile wallWE		= Tile('-', COLOR_WHITE,	"a stone wall");
Tile corridor	= Tile('#', COLOR_BLUE,		"a dark corridor", true);
Tile grass		= Tile(',', COLOR_GREEN,	"a patch of grass", true);
Tile tree		= Tile('T', COLOR_GREEN,	"a tree");
Tile log		= Tile('o', COLOR_RED,		"a log", true);
Tile player		= Tile('@', COLOR_YELLOW,	"you", true, false);

Yarl::Yarl(int argc, char *argv[])
{
	// initialize variables
	_variables =
	{
		{"color",		{"1",	"enable / disable color."}},
		{"visibleBold",	{"1",	"draw visible tiles bold."}},
		{"showUnknown",	{"0",	"draw unexplored areas."}},
		{"showUnseen",	{"1",	"draw explored,"
								"but currently not seen areas."}}
	};

	_bindings =
	{
		{'h', Command::west},
		{'l', Command::east},
		{'k', Command::north},
		{'j', Command::south},
		{'y', Command::northWest},
		{'u', Command::northEast},
		{'b', Command::southWest},
		{'n', Command::southEast},

		{'q', Command::quit}
	};

	// read variables from commandline
	if (argc % 2 == 0)
		argc --;

	for (int i = 1; i < argc; i += 2)
	{
		try
		{
			_variables[argv[i]] = argv[i + 1];
		}
		catch (logic_error)
		{}
	}

	// create test world
	Sector* s1 = Generator::generateGrassland();
	Sector* s2 = Generator::generateGrassland();
	s1->setNorth(s2);
	s1->setSouth(s2);
	s1->setWest(s2);
	s1->setEast(s2);
	s2->setNorth(s1);
	s2->setSouth(s1);
	s2->setWest(s1);
	s2->setEast(s1);

	_player = new Character(player, 42, 42, 5, 16, s1);

	Sector::setStatusBar(&_statusBar);
}

bool Yarl::init()
{
	// initialize curses
	initscr();
	cbreak();
	noecho();
	curs_set(0);

	// initialize colors
	if (has_colors() && _variables["color"].toInt())
	{
		start_color();
		for (int i = 1; i < 8; i++)
			init_pair(i, i, COLOR_BLACK);
	}

	return true;
}

void Yarl::render()
{
	// get window proportions
	int width, height;
	getmaxyx(stdscr, height, width);

	int offX = width / 2 - _player->x();
	int offY = height / 2 - _player->y();

	// render the room
	for (int row = 0; row < height; row++)
	{
		move(row, 0);
		for (int col = 0; col < width; col++)
		{
			Tile* t = _player->sector()->at(col - offX, row - offY);
			if (t != nullptr &&
				_player->los(col - offX, row - offY))
			{
				// render tiles the character has a LOS to
				_player->sector()->setExplored(col - offX, row - offY);

				attrset(COLOR_PAIR(t->color()) |
						// only draw bold if the variable is set
						(_variables["visibleBold"].toInt() ? A_BOLD : 0));
				addch(t->repr());
			}
			else if(t != nullptr &&
					(_variables["showUnknown"].toInt()	||
					 (_variables["showUnseen"].toInt()	&&
					  _player->sector()->explored(col - offX, row - offY))))
			{
				// tiles the player has seen before are rendered in grey
				attrset(COLOR_PAIR(COLOR_WHITE) | A_DIM);
				addch(t->repr());
			}
			else
			{
				addch(' ');
			}
		}
	}

	// render entities
	for (pair<pair<int, int>, Entity*> e :
		 _player->entitiesAround(_player->x(), _player->y(),
								 offX, offY, width, height))
	{
		if (_player->los(e.first.first - offX, e.first.second - offY))
		{
			e.second->setSeen();
			e.second->setLastKnownX();
			e.second->setLastKnownY();

			move(e.first.second, e.first.first);
			attrset(COLOR_PAIR(e.second->t().color()) |
					(_variables["visibleBold"].toInt() ? A_BOLD : 0));
			addch(e.second->t().repr());
		}
		else if (_variables["showUnknown"].toInt() ||
				 (_variables["showUnseen"].toInt() &&
				  e.second->seen()))
		{
			move(e.first.second, e.first.first);
			attrset(COLOR_PAIR(COLOR_WHITE) | A_DIM);
			addch(e.second->t().repr());
		}
	}

	// render status bar
	if (!_statusBar.empty())
	{
		attrset(COLOR_PAIR(COLOR_WHITE) | A_NORMAL);
		mvaddstr(0, 0, _statusBar.getLine(width).c_str());
		_moreMessages = !_statusBar.empty();
	}

	refresh();
}

bool Yarl::loop()
{
	char input = getch();

	Command cmd = _bindings[input];

	if (cmd == Command::quit)
		return true;

	if (_moreMessages)
		return false;

	else if (cmd > Command::MOVEMENT_BEGIN && cmd < Command::MOVEMENT_END)
	{
		int dx = 0;
		int dy = 0;

		if (cmd == Command::west ||
			cmd == Command::northWest ||
			cmd == Command::southWest)
			dx = -1;
		else if (cmd == Command::east ||
				 cmd == Command::northEast ||
				 cmd == Command::southEast)
			dx = 1;

		if (cmd == Command::north ||
			cmd == Command::northWest ||
			cmd == Command::northEast)
			dy = -1;
		else if (cmd == Command::south ||
				 cmd == Command::southWest ||
				 cmd == Command::southEast)
			dy = 1;

		if (!_player->move(dx, dy))
			_player->attack(dx, dy);
	}

	// continue main loop
	return false;
}

int Yarl::cleanup()
{
	endwin();

	return 0;
}

int Yarl::exec()
{
	if (!init())
	{
		return -1;
	}

	bool finished = false;

	while (!finished)
	{
		render();
		finished = loop();
	}

	return cleanup();
}
