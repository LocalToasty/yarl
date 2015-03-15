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

#include "item.h"

Entity::Entity(const Tile& t, int x, int y, int hp, Sector* sector,
			   const list<Item*>& inventory) :
	_t(t), _x(x), _y(y), _hp(hp), _sector(sector), _inventory(inventory)
{
	if (sector != nullptr)
		sector->addEntity(this);
}

Entity::~Entity()
{
	_sector->removeEntity(this);
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

Sector* Entity::sector() const
{
	return _sector;
}

bool Entity::seen() const
{
	return _seen;
}

int Entity::hp() const
{
	return _hp;
}

list<Item*>& Entity::inventory()
{
	return _inventory;
}

void Entity::setX(int x)
{
	setSector(_sector->sectorAt(x, _y));
	_x = Sector::mod(x);
}

void Entity::setY(int y)
{
	setSector(_sector->sectorAt(_x, y));
	_y = Sector::mod(y);
}

void Entity::setSector(Sector* sector)
{
	if (_sector != nullptr)
		_sector->removeEntity(this);

	_sector = sector;
	_sector->addEntity(this);
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
		_sector->statusBar()->
			addMessage("The " + _t.description() + " is destroyed.");
		// drop inventory
		for (Item* e : _inventory)
		{
			e->setSector(_sector);
			e->setX(_x);
			e->setY(_y);
			e->setSeen(false);
		}

		_sector->removeEntity(this);
	}
	_hp = hp;
}
