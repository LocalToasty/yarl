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

#ifndef ENTITY_H
#define ENTITY_H

#include "tile.h"
#include <list>

class Item;
class World;
class Sector;
class Character;

using namespace std;

class Entity
{
public:
	enum Size
	{
		colossal = -8,
		gargantuan = -4,
		huge = -2,
		large = -1,
		medium = 0,
		small = 1,
		tiny = 2,
		diminutive = 4,
		fine = 8
	};

private:
	const Tile& _t;

	int _x;
	int _y;

	int	_hp;	// hitpoints
	int _maxHp;

	int _naturalArmor;

	Size _s;

	World& _world;
	Sector* _sector;

	bool _seen {false};	// has the entity been seen yet?
	// if yes, last known coordinates
	int _lastKnownX;
	int _lastKnownY;

	list<Item*>	_inventory;

	Character* _lastAttacker;

public:
	Entity( const Tile& t, int x, int y, int hp, World& world,
			Size s = Size::medium, int naturalArmor = 0,
			const list<Item*>& inventory = {} );
	virtual ~Entity();


	virtual void think();
	virtual string dieMessage();

	const Tile&	t() const;

	int x() const;
	int y() const;
	World& world() const;
	Sector* sector() const;

	bool seen() const;
	int lastKnownX() const;
	int lastKnownY() const;

	int hp() const;
	Size size() const;
	int naturalArmor() const;
	virtual int armorClass() const;

	list<Item*>& inventory();

	void setX( int x );
	void setY( int y );
	void setSector( Sector* Sector );

	void setSeen( bool seen = true );
	void setLastKnownX();
	void setLastKnownY();

	void setHp(int hp);

	Character* lastAttacker() const;
	void setLastAttacker( Character* lastAttacker );
	int maxHp() const;
	void setMaxHp(int maxHp);
};
#endif
