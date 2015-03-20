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

#include "player.h"
#include "character.h"
#include "world.h"

Player::Player( const Tile& t, int x, int y, int hp, int visionRange,
				array<int, noOfAttributes>& attributes, World& world,
				int ( *unarmed )(), double unarmedRange,
				const list<Item*>& inventory, int bab, Size s,
				int naturalArmor ) :
	Character( t, x, y, hp, visionRange, attributes, world, unarmed,
			   unarmedRange, inventory, bab, s, naturalArmor )
{
}

string Player::attackMessage( Entity* target, bool hit )
{
	string msg = "You ";
	if( hit )
		msg += "hit";
	else
		msg += "miss";

	msg += " the " + target->t().description();

	if( weapon() )
		msg += " with your " + weapon()->t().description();

	msg += '.';

	return msg;
}

string Player::dieMessage()
{
	return "You die.";
}
