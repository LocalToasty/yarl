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

Character::Character(const Tile& t, int x, int y, int hp, int visionRange,
					 Sector* currentSector,
					 const list<Item*>& inventory) :
	Entity(t, x, y, hp, currentSector, inventory), _visionRange(visionRange)
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

bool Character::los(int x, int y, double factor) const
{
	return sector()->los(this->x(), this->y(), x, y, _visionRange * factor);
}

list<pair<pair<int, int>, Entity*>> Character::entitiesAround(int x, int y,
															  int offX,
															  int offY,
															  int width,
															  int height)
{
	return sector()->entitiesAround(x, y, offX, offY, width, height);
}
