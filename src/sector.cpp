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

#include "sector.h"

#include "entity.h"
#include <algorithm>	// for swap
#include <cmath>		// for abs

using namespace std;

const int Sector::_width	= 0x80;
const int Sector::_height	= 0x80;

Sector::Sector(Tile* defTile, Sector* north, Sector* south, Sector* west, Sector* east) :
	_tiles(_width * _height, defTile), _explored(_width * _height),
	_north(north), _south(south), _west(west), _east(east)
{
}

Sector::~Sector()
{
	for (Entity* e : _entities)
		delete e;
}

// returns true if a line of sight exists between the given points
bool Sector::los(int x1, int y1, int x2, int y2)
{
	if (x1 > _width || x2 > _width || y1 > _height || y2 > _height ||
		x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0)
	{
		return false;	// TODO: cross sector LOS
	}

	// list of all entities capable of blocking the line of sight
	list<Entity*> blockingEntities;
	for (Entity* e : _entities)
	{
		if (!e->t().opaque())
			blockingEntities.push_back(e);
	}

	const int dx = x2 - x1;
	const int dy = y2 - y1;

	const int dirX = (dx > 0) ? 1 : -1;
	const int dirY = (dy > 0) ? 1 : -1;

	if (abs(dx) > abs(dy))
	{
		int y = y1;
		int c = abs(dy);

		for (int x = x1; x != x2; x += dirX)
		{
			if (c > abs(dx))
			{
				y += dirY;
				c -= abs(dx);
			}

			// check if LOS is broken
			if (!at(x, y)->opaque())
				return false;

			// check if an entity blocks the Line of sight
			for (Entity* e : blockingEntities)
				if (e->x() == x && e->y() == y)
					return false;

			c += abs(dy);
		}
	}
	else
	{
		int x = x1;
		int c = abs(dx);

		for (int y = y1; y != y2; y += dirY)
		{
			if (c > abs(dy))
			{
				x += dirX;
				c -= abs(dy);
			}

			// check if LOS is broken
			if (!at(x, y)->opaque())
				return false;

			// check if an entity blocks the Line of sight
			for (Entity* e : blockingEntities)
				if (e->x() == x && e->y() == y)
					return false;

			c += abs(dx);
		}
	}

	return true;
}

bool Sector::passableAt(int x, int y)
{
	if (!at(x, y)->passable())
		return false;

	for (Entity* e : _entities)
		if (e->x() == x && e->y() == y && !e->t().passable())
			return false;

	return true;
}

int Sector::width() const
{
	return _width;
}

int Sector::height() const
{
	return _height;
}

const vector<Tile*>& Sector::tiles()
{
	return _tiles;
}

list<Entity*>& Sector::entities()
{
	return _entities;
}

bool Sector::explored(int x, int y)
{
	// if tile is in this sector
	if (x > 0 && x < _width && y > 0 && y < _height)
		// the tiles are stored linearly
		return _explored.at(x + _width * y);

	// if the x / y coordinates point into an adjacent sector
	else if (x < 0 && _west != nullptr)
		return _west->explored(x + _width, y);

	else if (y < 0 && _north != nullptr)
		return _north->explored(x, y + _height);

	else if (x > _width && _east != nullptr)
		return _east->explored(x - _width, y);

	else if (y > _height && _south != nullptr)
		return _south->explored(x, y - _height);

	else return false;	// no such tile
}

void Sector::setExplored(int x, int y, bool explored)
{
	// if tile is in this sector
	if (x > 0 && x < _width && y > 0 && y < _height)
		// the tiles are stored linearly
		_explored.at(x + _width * y) = explored;

	// if the x / y coordinates point into an adjacent sector
	else if (x < 0 && _west != nullptr)
		_west->setExplored(x + _width, y, explored);

	else if (y < 0 && _north != nullptr)
		_north->setExplored(x, y + _height, explored);

	else if (x > _width && _east != nullptr)
		_east->setExplored(x - _width, y, explored);

	else if (y > _height && _south != nullptr)
		_south->setExplored(x, y - _height, explored);
}

Tile* Sector::at(int x, int y)
{
	// if tile is in this sector
	if (x > 0 && x < _width && y > 0 && y < _height)
		// the tiles are stored linearly
		return _tiles.at(x + _width * y);

	// if the x / y coordinates point into an adjacent sector
	else if (x < 0 && _west != nullptr)
		return _west->at(x + _width, y);

	else if (y < 0 && _north != nullptr)
		return _north->at(x, y + _height);

	else if (x > _width && _east != nullptr)
		return _east ->at(x - _width, y);

	else if (y > _height && _south != nullptr)
		return _south->at(x, y - _height);

	else return nullptr;	// no such tile
}

void Sector::setAt(int x, int y, Tile* tile)
{
	_tiles.at(x + _width * y) = tile;
}

Sector* Sector::north() const
{
	return _north;
}

void Sector::setNorth(Sector* north)
{
	_north = north;
}

Sector* Sector::south() const
{
	return _south;
}

void Sector::setSouth(Sector* south)
{
	_south = south;
}

Sector* Sector::west() const
{
	return _west;
}

void Sector::setWest(Sector* west)
{
	_west = west;
}

Sector* Sector::east() const
{
	return _east;
}

void Sector::setEast(Sector* east)
{
	_east = east;
}

void Sector::hLine(int x, int y, int len, Tile* tile)
{
	for (int col = x; col < x + len; col++)
		setAt(col, y, tile);
}

void Sector::vLine(int x, int y, int len, Tile* tile)
{
	for (int row = y; row < y + len; row++)
		setAt(x, row, tile);
}
void Sector::square(int x, int y,
					int width, int height,
					Tile* tile)
{
	for (int row = y; row < y + height; row++)
		for (int col = x; col < x + width; col++)
			setAt(col, row, tile);
	
}

void Sector::createRoom(int x, int y,
						int width, int height,
						Tile* ground, Tile* hWall,
						Tile* vWall)
{
	// north wall
	hLine(x, y, width, hWall);
	// south wall
	hLine(x, y + height - 1, width, hWall);

	// west wall
	vLine(x, y + 1, height - 2, vWall);
	// east wall
	vLine(x + width - 1, y + 1, height - 2, vWall);

	// ground
	square(x + 1, y + 1, width - 2, height - 2, ground);
}
