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

#include "entity.h"
#include "world.h"
#include "sector.h"

#include "item.h"

Entity::Entity( const Tile& t, int x, int y, int hp, World& world, Size s,
				int naturalArmor, const list<Item*>& inventory ) :
	_t( t ), _x( x ), _y( y ), _hp( hp ), _world( world ), _s( s ),
	_naturalArmor( naturalArmor ), _inventory( inventory )
{
	_sector = world.sector( x, y );

	if( _sector != nullptr)
		_sector->addEntity( this );
}

Entity::~Entity()
{
	if( _sector != nullptr )
		_sector->removeEntity(this);
}

int Entity::armorClass() const
{
	return 5 + _s + _naturalArmor;
}

void Entity::think()
{
}

string Entity::dieMessage()
{
	return "The " + t().description() + " is destroyed.";
}

const Tile& Entity::t() const
{
	return _t;
}

int Entity::x() const
{
	return _x;
}

int Entity::y() const
{
	return _y;
}

World& Entity::world() const
{
	return _world;
}

Sector* Entity::sector() const
{
	return _sector;
}

bool Entity::seen() const
{
	return _seen;
}

int Entity::lastKnownX() const
{
	return _lastKnownX;
}

int Entity::lastKnownY() const
{
	return _lastKnownY;
}

int Entity::hp() const
{
	return _hp;
}

Entity::Size Entity::size() const
{
	return _s;
}

int Entity::naturalArmor() const
{
	return _naturalArmor;
}

list<Item*>& Entity::inventory()
{
	return _inventory;
}

void Entity::setX( int x )
{
	_x = x;
	setSector(_world.sector(_x, _y));
}

void Entity::setY(int y)
{
	_y = y;
	setSector(_world.sector(_x, _y));
}

void Entity::setSector( Sector* sector )
{
	if( _sector != nullptr )
		_sector->removeEntity( this );

	if( sector != nullptr )
		sector->addEntity( this );

	_sector = sector;
}

void Entity::setSeen(bool seen)
{
	_seen = seen;
}

void Entity::setLastKnownX()
{
	_lastKnownX = _x;
}

void Entity::setLastKnownY()
{
	_lastKnownY = _y;
}

void Entity::setHp(int hp)
{
	if (hp <= 0)
	{
		_world.statusBar().addMessage( dieMessage() );
		// drop inventory
		for (Item* e : _inventory)
		{
			e->setX(_x);
			e->setY(_y);
			e->setSeen(false);
		}

		_sector->removeEntity(this);
	}
	_hp = hp;
}
