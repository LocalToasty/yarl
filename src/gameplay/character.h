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
#include "armor.h"
#include <array>

using namespace std;

class Character : public Entity
{
public:
	enum Attribute
	{
		strength,
		dexterity,
		constitution,
		intelligence,
		wisdom,
		charisma,
		noOfAttributes
	};

private:
	int _visionRange;

	Weapon* _weapon { nullptr };

	// unarmed attack
	int ( *_unarmed )();
	double _unarmedRange;

	Armor* _armor { nullptr };
	Armor* _shield { nullptr };

	array<int, noOfAttributes> _attributes;

	int _bab;	// base attack bonus

	Entity* _lastTarget { nullptr };

public:
	Character( const Tile& t, int x, int y, int hp, int visionRange,
			   const array<int, noOfAttributes>& attributes, World& world,
			   int ( *unarmed )(), double unarmedRange = 1.5,
			   const list<Item*>& inventory = {},
			   int bab = 0, Size s = Size::medium, int naturalArmor = 0 );

	bool los( int x, int y, double factor = 1 ) const;
	bool los( Entity* e );
	vector<Entity*> seenEntities();

	bool move( int dx, int dy );

	void attack( Entity* target );

	virtual string attackMessage( Entity* target, bool hit );
	virtual string dieMessage();

	int armorClass();

	Weapon* weapon();
	void setWeapon( Weapon* weapon );

	Armor* armor();
	void setArmor( Armor* armor );

	Armor* shield() const;
	void setShield(Armor* shield);

	int attribute( Attribute attribute );
	void setAttribute( Attribute attribute, int value );
	int attributeMod( Attribute attribute );

	int visionRange();

	Entity* lastTarget() const;
	void setLastTarget(Entity* lastTarget);
	double unarmedRange() const;
	void setUnarmedRange(double unarmedRange);
};

#endif
