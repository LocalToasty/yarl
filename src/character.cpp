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

#include "character.h"
#include "sector.h"

Character::Character(const Tile& t, int x, int y, int hp, Sector* currentSector,
					 const list<Item*>& inventory) :
	Entity(t, x, y, hp, currentSector, inventory)
{
}

bool Character::move(int dx, int dy)
{
	if (sector()->passableAt(x() + dx, y() + dy))
	{
		setX(x() + dx);
		setY(y() + dy);
		return true;
	}

	return false;
}

void Character::attack(int dx, int dy)
{
	list<Entity*> targets = sector()->entitiesAt(x() + dx, y() + dy);

	for (Entity* e : targets)
	{
		attack(e);
	}
}

void Character::attack(Entity* target)
{
	target->setHp(target->hp() - 1);
}

bool Character::los(int x, int y) const
{
	return sector()->los(this->x(), this->y(), x, y);
}

bool Character::los(Entity* e) const
{
	return sector()->los(x(), y(), e->x(), e->y());
}
