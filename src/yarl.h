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

#ifndef YARL_H
#define YARL_H

#include "sector.h"
#include "character.h"
#include "tile.h"
#include "variable.h"
#include "statusBar.h"
#include <string>
#include <map>

using namespace std;

class Yarl
{
private:
	enum class Command
	{
		MOVEMENT_BEGIN,
		north,
		south,
		west,
		east,
		northWest,
		northEast,
		southWest,
		southEast,
		MOVEMENT_END,

		pickUp,

		quit
	};

private:
	Character* _player;

	StatusBar _statusBar;

	map<string, Variable> _variables;
	map<char, Command> _bindings;

	bool _moreMessages {false};

private:
	bool init();

	void render();
	bool loop();

	int cleanup();

public:
	Yarl(int argc, char* argv[]);

	int exec();
};

#endif
