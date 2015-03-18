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

Character::Character( const Tile& t, int x, int y, int hp, int visionRange,
					  int st, int dx, int in, int bab, Weapon* unarmed,
					 World* world,
					 const list<Item*>& inventory) :
	Entity( t, x, y, hp, world, inventory ), _bab( bab ),
	_unarmed( unarmed ), _visionRange(visionRange)
{
	_attributes[Attribute::strength] = st;
	_attributes[Attribute::dexterity] = dx;
	_attributes[Attribute::intelligence] = in;
}

bool Character::move( int dx, int dy )
{
	if ( world()->passable( x() + dx, y() + dy ) )
	{
		setX( x() + dx );
		setY( y() + dy );
		return true;
	}

	return false;
}

void Character::attack( Entity* target )
{
	// hit roll
	if( rand() % 20 + 1 >= target->armorClass() )
	{
		world()->statusBar().addMessage( attackMessage( target, true ) );

		int damage = ( _weapon == nullptr  ? _unarmed->damage()
										   : _weapon->damage()) +
					 attributeMod( strength );

		if( damage <= 0 )	// hits inflict at least 1 hp damage
			damage = 1;

		target->setHp( target->hp() - damage );
		return;
	}
	else	// don't do any damage on miss
		world()->statusBar().addMessage( attackMessage( target, false ) );
}

string Character::attackMessage( Entity* target, bool hit )
{
	string msg = "The " + t().description();
	if( hit )
		msg += " hits";
	else
		msg += " misses";

	msg += " the " + target->t().description() + '.';

	return msg;
}

string Character::dieMessage()
{
	return "The " + t().description() + " dies.";
}

bool Character::los(int x, int y, double factor) const
{
	return world()->los( this->x(), this->y(), x, y, _visionRange * factor );
}

bool Character::los( Entity* e )
{
	return los( e->x(), e->y() );
}

int Character::armorClass()
{
	return 10 + attributeMod( dexterity );
}

int Character::modifier( int base )
{
	return ( base - 10 ) / 2;
}

Weapon* Character::weapon()
{
	return _weapon;
}

void Character::setWeapon( Weapon* weapon )
{
	_weapon = weapon;
}

int Character::attribute(Character::Attribute attribute)
{
	return _attributes[attribute];
}

int Character::attributeMod(Character::Attribute attribute)
{
	return modifier( _attributes[attribute] );
}

int Character::visionRange()
{
	return _visionRange;
}
