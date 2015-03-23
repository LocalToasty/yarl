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

#ifndef SECTOR_H
#define SECTOR_H

#include "tile.h"
#include "command.h"
#include <vector>
#include <list>

using namespace std;

class Tile;

// declaration to not run into recursion issiues
class Entity;

class Sector
{
private:
	// size of a sector has to be hardwired so they can be tiled
	static const int _size;

	// vector containing the tiles (stored linearly row for row)
	vector<Tile*> _tiles;
	vector<bool> _explored;

	// a list of all entities in the sector (i.e. characters, items, props)
	// the bottommost entity has highest render priority
	list<Entity*> _entities;

public:
	Sector(Tile* defTile);
	~Sector();

	static int size();

	const list<Entity*>& entities() const;
	vector<Entity*> entities( int x, int y ) const;
	void addEntity( Entity* e );
	void removeEntity( Entity* e );

	Tile* tile( int x, int y );
	void setTile( int x, int y, Tile* tile );

	bool passable( int x, int y );

	bool explored( int x, int y );
	void setExplored( int x, int y, bool explored = true );
};

#endif
