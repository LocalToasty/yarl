/*
 * YARL - Yet another Roguelike
 * Copyright (C) 2015  Marko van Treeck
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

#include "globals.h"
#include "sector.h"
#include "character.h"
#include "tile.h"
#include "variable.h"
#include <string>
#include <map>

using namespace std;

class Yarl
{
private:
	Sector*		_currentSector;
	Character*	_player;

	map<string, Variable>	_variables;

private:
	bool init();

	void render();
	bool loop();

	void cleanup();

public:
	Yarl(int argc, char* argv[]);

	int exec();
};

#endif
