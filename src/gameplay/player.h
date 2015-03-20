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

#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include "weapon.h"
#include <string>

using namespace std;

class Player : public Character
{
public:
	Player( const Tile& t, int x, int y, int hp, int visionRange,
			array<int, noOfAttributes>& attributes, World& world,
			int ( *unarmed )(), double unarmedRange = 1.5,
			const list<Item*>& inventory = {}, int bab = 0,
			Size s = Size::medium, int naturalArmor = 0 );

	string attackMessage( Entity* target, bool hit );
	string dieMessage();
};

#endif
