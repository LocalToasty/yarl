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

#include "character.h"
#include "world.h"
#include "sector.h"
#include "yarlconfig.h"
#include <cstdlib>

using namespace std;

Character::Character( const Tile& t, int x, int y, int hp, double speed,
					  int visionRange,
                      const array<int, noOfAttributes>& attributes,
					  World& world, Attack* unarmed,
					  const list<Item*>& inventory,
					  int bab, Character:: Size s, int naturalArmor ) :
	Entity( t, x, y, hp, world, s, naturalArmor, inventory ), _speed( speed ),
	_attributes( attributes ), _bab( bab ), _unarmed( unarmed ),
	_visionRange( visionRange )
{
}

bool Character::move( int dx, int dy )
{
	if ( world().passable( x() + dx, y() + dy ) )
	{
		setXY( x() + dx, y() + dy );
		return true;
	}

	return false;
}

void Character::attack( Entity* target )
{
	_lastTarget = target;
	target->setLastAttacker( this );

	// hit roll
	int toHitMod = _bab + attributeMod( strength ) + size();
	if( World::distance( x(), y(), target->x(), target->y() ) <=
		_unarmed->range() &&
		rand() % 20 + 1 + toHitMod >= target->armorClass() )
	{
		world().statusBar().addMessage( attackMessage( target, true ) );

		int damage = _unarmed->damage() + attributeMod( strength );

		if( damage <= 0 )	// hits inflict at least 1 hp damage
			damage = 1;

		target->setHp( target->hp() - damage );
		return;
	}
	else	// don't do any damage on miss
		world().statusBar().addMessage( attackMessage( target, false ) );
}

string Character::attackMessage( Entity* target, bool hit, Weapon* w )
{
	string msg = "The " + desc();
	if( hit )
		msg += " hits";
	else
		msg += " misses";

	msg += " the " + target->desc() + '.';

	return msg;
}

string Character::dieMessage()
{
	return "The " + desc() + " dies.";
}

bool Character::los(int x, int y, double factor) const
{
	return world().los( this->x(), this->y(), x, y, _visionRange * factor );
}

bool Character::los( Entity* e )
{
	return los( e->x(), e->y() );
}

// returns a vector with the entities currently seen by the character
vector<Entity*> Character::seenEntities()
{
	vector<Entity*> ents;

	for( Entity* e : world().entities( x() - visionRange(),
									   y() - visionRange(),
									   x() + visionRange() + 1,
									   y() + visionRange() + 1 ) )
	if( los( e ) )
			ents.push_back( e );

	return ents;
}

int Character::armorClass()
{
	return 10 + attributeMod( dexterity ) + size() + naturalArmor() +
			( _armor == nullptr ? 0 : _armor->ac() );
}

Attack* Character::unarmed()
{
	return _unarmed;
}

Armor* Character::armor()
{
	return _armor;
}

void Character::setArmor( Armor* armor )
{
	_armor = armor;
}

int Character::attribute( Character::Attribute attribute )
{
	return _attributes[attribute];
}

int Character::attributeMod( Character::Attribute attribute )
{
	int bonus = ( _attributes[attribute] - 10 ) / 2;

	if( _armor )
	{
		if( ( attribute == dexterity || attribute == strength ) )
			bonus += _armor->checkPenalty();
		if( attribute == dexterity && bonus > _armor->maxDexBon() )
			bonus = _armor->maxDexBon();
	}

	return bonus;
}

int Character::visionRange()
{
	return _visionRange;
}

double Character::speed()
{
	return _speed;
}

Entity* Character::lastTarget() const
{
	return _lastTarget;
}

void Character::setLastTarget( Entity* lastTarget )
{
	_lastTarget = lastTarget;
}

int Character::bab()
{
	return _bab;
}