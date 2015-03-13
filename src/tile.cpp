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

#include "tile.h"

bool Tile::transparent() const
{
	return _transparent;
}

void Tile::setTransparent(bool transparent)
{
	_transparent = transparent;
}

Tile::Tile(char repr, short color, string description, bool transparent) :
	_repr(repr), _color(color), _description(description),
	_transparent(transparent), _passable(transparent)
{}

Tile::Tile(char repr, short color, string description,
		   bool transparent, bool passable) :
	_repr(repr), _color(color), _description(description),
	_transparent(transparent), _passable(passable)
{
}

char Tile::repr() const
{
	return _repr;
}

short Tile::color() const
{
	return _color;
}

string Tile::description() const
{
	return _description;
}

bool Tile::passable() const
{
	return _passable;
}

void Tile::setRepr(char repr)
{
	_repr = repr;
}

void Tile::setColor(short color)
{
	_color = color;
}

void Tile::setPassable(bool passable)
{
	_passable = passable;
}
