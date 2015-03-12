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

#ifndef SECTOR_H
#define SECTOR_H

#include "globals.h"
#include "tile.h"
#include <vector>
#include <list>

using namespace std;

// declaration to not run into recursion issiues
class Entity;
class Generator;

class Sector
{
	friend class Generator;

private:
	// size of a sector has to be hardwired so they can be tiled
	static const int _size;

private:
	// vector containing the tiles (stored linearly row for row)
	vector<Tile*> _tiles;
	vector<bool> _explored;

	// a list of all entities in the sector (i.e. characters, items, props)
	// the bottommost entity has highest render priority
	list<Entity*> _entities;

	// pointer to adjacent sectors
	Sector*	_north;
	Sector*	_south;
	Sector*	_west;
	Sector* _east;

public:
	Sector(Tile* defTile,
		   Sector* north = nullptr, Sector* south = nullptr,
		   Sector* west = nullptr, Sector* east = nullptr);
	~Sector();

	bool los(int x1, int y1, int x2, int y2, double max = -1);
//	list<Command> route(int x1, int y1, int x2, int y2);	// TODO
	bool passableAt(int x, int y);
	Sector* sectorAt(int x, int y);
	static int mod(int i);

	static int size();

	const vector<Tile*>& tiles();	// returns all tiles

	const list<Entity*>& entities() const;
	list<Entity*> entitiesAt(int x, int y);
	list<pair<pair<int, int>, Entity*>> entitiesAround(int x, int y,
													   int rw, int re,
													   int rn, int rs,
													   int offX, int offY);

	void addEntity(Entity* e);
	void removeEntity(Entity* e);

	bool explored(int x, int y);
	void setExplored(int x, int y, bool explored = true);

	// getters and setters for adjacent sectors
	Sector* north()	const;
	Sector* south()	const;
	Sector* west()	const;
	Sector* east()	const;

	void setNorth(Sector* north);
	void setSouth(Sector* south);
	void setWest(Sector* west);
	void setEast(Sector* east);
	
	// returns the tile at the given location
	Tile* at(int x, int y);
	void setAt(int x, int y, Tile* tile);
};

#endif	// SECTOR_H
