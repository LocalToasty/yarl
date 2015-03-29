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

#include "companion.h"
#include "world.h"
#include <cstdlib>

using namespace std;

Companion::Companion( const Tile& t, Character* companion,
					  int hp, int x, int y, double speed, int visionRange,
					  const array<int, Character::noOfAttributes>& attributes,
					  World& world, Attack* unarmed,
					  const list<Item*>& inventory, int bab, Entity::Size s,
					  int naturalArmor ) :
	NPC( t, hp, x, y, speed, visionRange, attributes, world, unarmed,
		 inventory, bab, s, naturalArmor ),
	_companion( companion )
{
}

void Companion::think()
{
	if( lastAttacker() != nullptr )
	{
		setLastTarget( lastAttacker() );

		if( lastTarget() == _companion )
			_companion = nullptr;
	}
	else if( _companion != nullptr )
	{
		if( _companion->lastTarget() != nullptr )
			setLastTarget( _companion->lastTarget() );
		else if( _companion->lastAttacker() != nullptr )
			setLastTarget( _companion->lastAttacker() );
	}

	if( lastTarget() != nullptr &&
		lastTarget()->hp() > 0 && los( lastTarget() ) )
	{
		_waypointX = lastTarget()->x();
		_waypointY = lastTarget()->y();
	}
	else if( _companion != nullptr && los( _companion ) )
	{
		_waypointX = _companion->x() + ( rand() % 9 ) - 4;
		_waypointY = _companion->y() + ( rand() % 9 ) - 4;
	}

	if( _waypointX >= 0 && _waypointY >= 0 )
	{
		if( World::distance( x(), y(), _waypointX, _waypointY ) >
			unarmed()->range() )
		{
			auto dir = world().route( x(), y(), _waypointX, _waypointY, true );
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
			setLastAction( lastAction() +
						   ( abs( dx ) + abs( dy ) == 1 ? speed()
														: 1.5 * speed() ) );
		}
		else if( lastTarget() != nullptr && lastTarget()->hp() > 0 )
		{
			attack( lastTarget() );
			setLastAction( lastAction() + 2 );
		}
	}
	else
	{
		setLastAction( world().time() );
	}
}
