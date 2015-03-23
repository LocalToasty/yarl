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

#include "weapon.h"

Weapon::Weapon( const Tile& t, int (*damage)(), World& world , double range,
				int x, int y, int hp , Size s ) :
	Item( t, world, x, y, hp, s ), _range( range ), _damage( damage )
{
}

int Weapon::damage()
{
	return _damage();
}

double Weapon::range()
{
	return _range;
}
