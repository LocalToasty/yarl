/*
 * YARL - Yet another Roguelike
 * Copyright (C) 2015  Marko van Treeck
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

#ifndef ENTITY_H
#define ENTITY_H

#include "sector.h"
#include "tile.h"
#include <list>

class Item;

using namespace std;

class Entity
{
private:
	const Tile& _t;

	int _x;
	int _y;

	int	_hp;	// hitpoints

	Sector* _sector;

	bool _seen {false};	// has the entity been seen yet?
	// if yes, last known coordinates
	int _lastKnownX;
	int _lastKnownY;

	list<Item*>	_inventory;

public:
	Entity(const Tile& t, int x, int y, int hp, Sector* sector = nullptr,
		   const list<Item*>& inventory = {});
	~Entity();

	const Tile&	t() const;

	int x() const;
	int y() const;
	Sector* sector() const;

	bool seen() const;
	int lastKnownX() const;
	int lastKnownY() const;

	int hp() const;

	list<Item*>& inventory();

	void setX(int x);
	void setY(int y);
	void setSector(Sector* Sector);

	void setSeen(bool seen = true);
	void setLastKnownX();
	void setLastKnownY();

	void setHp(int hp);
};
#endif
