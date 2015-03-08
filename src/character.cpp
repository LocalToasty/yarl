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
					 const list<Entity*>& inventory) :
	Entity(t, x, y, hp, currentSector, inventory)
{
}

void Character::move(Command dir)
{
	switch (dir) {
	case Command::west:
		if (sector()->passableAt(x() - 1, y()))
			setX(x() - 1);
		break;

	case Command::east:
		if (sector()->passableAt(x() + 1, y()))
			setX(x() + 1);
		break;

	case Command::north:
		if(sector()->passableAt(x(), y() - 1))
			setY(y() - 1);
		break;

	case Command::south:
		if (sector()->passableAt(x(), y() + 1))
			setY(y() + 1);
		break;

	case Command::northWest:
		if (sector()->passableAt(x() - 1, y() - 1))
		{
			setX(x() - 1);
			setY(y() - 1);
		}
		break;

	case Command::northEast:
		if (sector()->passableAt(x() + 1, y() - 1))
		{
			setX(x() + 1);
			setY(y() - 1);
		}
		break;

	case Command::southWest:
		if (sector()->passableAt(x() - 1, y() + 1))
		{
			setX(x() - 1);
			setY(y() + 1);
		}
		break;

	case Command::southEast:
		if (sector()->passableAt(x() + 1, y() + 1))
		{
			setX(x() + 1);
			setY(y() + 1);
		}
		break;

	}
}

bool Character::los(int x, int y) const
{
	return sector()->los(this->x(), this->y(), x, y);
}

bool Character::los(Entity* other) const
{
	return sector()->los(x(), y(), other->x(), other->y());
}
