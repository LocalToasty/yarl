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

#ifndef TILE_H
#define TILE_H

#include "iomanager.h"
#include <string>

using namespace std;

class Tile
{
private:
	char _repr;
	Color _color;

	string _prefix;
	string _description;

	bool _transparent;
	bool _passable;

public:
	Tile(char repr = ' ', Color color = Color::black, string pref = "a",
		  string description = "", bool transparent = false,
		  bool passable = false);

	char repr() const;
	Color color() const;
	string prefix() const;
	string desc() const;
	bool passable() const;
	bool transparent() const;
};

#endif
