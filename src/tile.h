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

#include <string>
#include <curses.h>

using namespace std;

class Tile
{
private:
	char	_repr;
	short	_color;
	string	_description;
	bool	_transparent;
	bool	_passable;

public:
	Tile(char repr = ' ', short color = COLOR_BLACK, string description = "",
		 bool transparent = false);
	Tile(char repr, short color, string description, bool transparent,
		 bool passable);

	char	repr()			const;
	short	color()			const;
	string	description()	const;
	bool	passable()		const;
	bool	transparent()		const;

	void	setRepr(char repr);
	void	setColor(short color);
	void	setDescription(string description);
	void	setPassable(bool passable);
	void	setTransparent(bool transparent);
};

#endif
