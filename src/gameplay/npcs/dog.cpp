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

#include "dog.h"
#include "world.h"
#include "entity.h"
#include "item.h"
#include "character.h"
#include "bite.h"
#include <cstdlib>

Tile Dog::dog = { 'd', Color::red, "dog", true, false };
Tile Dog::corpse = { '%', Color::red, "dog corpse", true };

Bite Dog::bite = {};

Dog::Dog( int x, int y, World* world )
	: NPC( dog, x, y, 6, 13, 13, 2, 0, &bite, 12, world,
		   { new Item( corpse, -1, -1, 1, world ) } )
{

}

void Dog::think()
{
	for( Entity* e : world()->entities( x() - visionRange(),
										y() - visionRange(),
										x() + visionRange() + 1,
										y() + visionRange() + 1 ) )
	{
		Character* c = dynamic_cast<Character*>( e );
		if( c != nullptr && c != this && los( e ) )
		{
			_waypointX = e->x() + ( rand() % 9 ) - 4;
			_waypointY = e->y() + ( rand() % 9 ) - 4;
			break;
		}
	}

	auto dir = world()->route( x(), y(),
							   _waypointX, _waypointY, true );
	Command cmd = dir.front();

	int dx = 0;
	int dy = 0;

	if( cmd == Command::west ||
		cmd == Command::northWest ||
		cmd == Command::southWest )
		dx = -1;
	else if( cmd == Command::east ||
			 cmd == Command::northEast ||
			 cmd == Command::southEast )
		dx = 1;

	if( cmd == Command::north ||
		cmd == Command::northWest ||
		cmd == Command::northEast )
		dy = -1;
	else if( cmd == Command::south ||
			 cmd == Command::southWest ||
			 cmd == Command::southEast )
		dy = 1;

	move( dx, dy );
}
