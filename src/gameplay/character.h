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

#ifndef CHARACTER_H
#define CHARACTER_H

#include "entity.h"
#include "tile.h"
#include "sector.h"
#include "weapon.h"

class Character : public Entity
{
public:
	enum Attribute
	{
		strength,
		dexterity,
		intelligence,
		noOfAttributes
	};

private:
	int _visionRange;

	Weapon* _weapon { nullptr };
	Weapon* _unarmed;

	int _attributes[noOfAttributes];

	int _bab;	// base attack bonus

	static int modifier( int base );

public:
	Character( const Tile& t, int x, int y, int hp, int visionRange,
			   int st, int dx, int in, int bab, Weapon* unarmed, World* world,
			   const list<Item*>& inventory = {} );

	bool los( int x, int y, double factor = 1 ) const;
	bool los( Entity* e );

	bool move( int dx, int dy );

	void attack( Entity* target );
	virtual string attackMessage( Entity* target, bool hit );
	string dieMessage();

	int armorClass();

	Weapon* weapon();
	void setWeapon( Weapon* weapon );

	int attribute( Attribute attribute );
	int attributeMod( Attribute attribute );

	int visionRange();
};

#endif
