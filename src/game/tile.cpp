/*
 * YARL - Yet another Roguelike
 * Copyright (C) 2015-2016  Marko van Treeck <markovantreeck@gmail.com>
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

Tile::Tile(char repr, Color color, string prefix, string description,
           bool transparent, bool passable)
    : _repr(repr),
      _color(color),
      _prefix(prefix),
      _description(description),
      _transparent(transparent),
      _passable(passable) {}

bool Tile::transparent() const { return _transparent; }

char Tile::repr() const { return _repr; }

Color Tile::color() const { return _color; }

string Tile::prefix() const { return _prefix; }

string Tile::desc() const { return _description; }

bool Tile::passable() const { return _passable; }
