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

#ifndef NPC_H
#define NPC_H

#include "character.h"
#include "command.h"

class NPC : public Character
{
public:
	NPC( const Tile& t, int x, int y, int hp, int visionRange,
		 const array<int, noOfAttributes>& attributes, Weapon* unarmed,
		 World& world, const list<Item*>& inventory = {}, int bab = 0,
		 Size s = Size::medium, int naturalArmor = 0 );

	virtual void think() = 0;
};

#endif
