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

class Character : public Entity
{
private:
	int _visionRange;

public:
	Character(const Tile& t, int x, int y, int hp, int visionRange,
			  Sector* currentSector = nullptr,
			  const list<Item*>& inventory = {});

	bool move(int dx, int dy);

	void attack(int dx, int dy);
	void attack(Entity* target);

	bool los(int x, int y, double factor = 1) const;

	list<pair<pair<int, int>, Entity*>> entitiesAround(int rx, int y,
													   int offX, int offY,
													   int width, int height);
};

#endif
