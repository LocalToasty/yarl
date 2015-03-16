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

Character::Character(const Tile& t, int x, int y, int hp, int visionRange,
					 World* world, const list<Item*>& inventory) :
	Entity(t, x, y, hp, world, inventory), _visionRange(visionRange)
{
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

void Character::attack(int dx, int dy)
{
	vector<Entity*> targets = world()->entities( x() + dx, y() + dy );

	for (Entity* e : targets)
	{
		attack(e);
	}
}

void Character::attack(Entity* target)
{
	world()->statusBar().addMessage("You attack the " +
									  target->t().description() + '.');
	target->setHp(target->hp() - 1);
}

bool Character::los(int x, int y, double factor) const
{
	return world()->los( this->x(), this->y(), x, y, _visionRange * factor );
}

bool Character::los(Entity* e)
{
	return los( e->x(), e->y() );
}
