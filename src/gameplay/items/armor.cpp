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

#include "armor.h"

Armor::Armor( Tile& t, int ac, int maxDexBon, bool shield, World& world,
			  int x, int y, int hp, Size s )
	: Item( t, world, x, y, hp, s ), _ac( ac ), _maxDexBon( maxDexBon ),
	  _shield( shield )
{
}

int Armor::ac()
{
	return _ac;
}

int Armor::maxDexBon()
{
	return _maxDexBon;
}

int Armor::shield() const
{
	return _shield;
}
