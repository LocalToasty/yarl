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

#include "sector.h"

#include "entity.h"
#include <algorithm>
#include <cmath>

using namespace std;

const int Sector::_size = 0x40;

StatusBar* Sector::_statusBar = nullptr;

Sector* Sector::sectorAt(int x, int y)
{
	if (x >= 0 && y >= 0 && x < _size && y < _size)
	{
		return this;
	}
	// if the x / y coordinates point into an adjacent sector
	else if (x < 0 && _west != nullptr)
	{
		return _west->sectorAt(x + _size , y);
	}
	else if (y < 0 && _north != nullptr)
	{
		return _north->sectorAt(x, y + _size);
	}
	else if (x >= _size && _east != nullptr)
	{
		return _east->sectorAt(x - _size, y);
	}
	else if (y >= _size && _south != nullptr)
	{
		return _south->sectorAt(x, y - _size);
	}
	else
		return nullptr;
}

int Sector::mod(int i)
{
	i %= _size;

	return (i >= 0) ? i : _size + i;
}

Sector::Sector(Tile* defTile, Sector* north, Sector* south,
			   Sector* west, Sector* east) :
	_tiles(_size * _size, defTile), _explored(_size * _size),
	_north(north), _south(south), _west(west), _east(east)
{
}

Sector::~Sector()
{
	for (Entity* e : _entities)
		delete e;
}

// returns true if a line of sight exists between the given points
bool Sector::los(int x1, int y1, int x2, int y2, double max)
{
	if(max > 0 && sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)) > max)
		return false;

	const int dx = x2 - x1;
	const int dy = y2 - y1;

	const int dirX = (dx > 0) ? 1 : -1;
	const int dirY = (dy > 0) ? 1 : -1;

	// coordinates are normalised to the sector of (x1,x2)
	Sector* s = sectorAt(x1, y1);
	x1 = mod(x1);
	y1 = mod(y1);
	x2 = x1 + dx;
	y2 = y1 + dy;

	// first contains normalised coordinates of entity
	list<pair<pair<int, int>, Entity*>> blockingEntities;
	for (Entity* e : s->_entities)
	{
		if (!e->t().transparent())
			blockingEntities.push_back({{e->x(), e->y()}, e});
	}

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
			if (!at(x, y)->transparent())
				return false;

			// check if an entity blocks the Line of sight
			for (pair<pair<int, int>, Entity*> e : blockingEntities)
				if (e.first.first == x && e.first.second == y)
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
			if (!at(x, y)->transparent())
				return false;

			// check if an entity blocks the Line of sight
			for (pair<pair<int, int>, Entity*> e : blockingEntities)
				if (e.first.first == x && e.first.second == y)
					return false;

			c += abs(dx);
		}
	}

	return true;
}

// returns true if neither the terrain nor the any entities at the coordinates
// are implassable.
bool Sector::passableAt(int x, int y)
{
	Sector* s = sectorAt(x, y);
	// if the sector at (x1,y1) doesn't exist, the terrain is impassable
	if (s == nullptr)
		return false;

	x = mod(x);
	y = mod(y);

	// check for terrain passability
	if (!s->at(x, y)->passable())
		return false;

	// check for entitiy passability
	for (Entity* e : s->entitiesAt(x, y))
		if (!e->t().passable())
			return false;

	return true;
}

int Sector::size()
{
	return _size;
}

const vector<Tile*>& Sector::tiles()
{
	return _tiles;
}

const list<Entity*>& Sector::entities() const
{
	return _entities;
}

void Sector::addEntity(Entity* e)
{
	auto pos = lower_bound(_entities.begin(), _entities.end(), e,
						   [](Entity* l, Entity* r){
							return !r->t().passable() &&
									l->t().passable();});

	_entities.insert(pos, e);
}

void Sector::removeEntity(Entity* e)
{
	_entities.remove(e);
}

list<Entity*> Sector::entitiesAt(int x, int y)
{
	Sector* s = sectorAt(x, y);
	x = mod(x);
	y = mod(y);

	// if tile is in this sector
	if (s != nullptr)
	{
		list<Entity*> ent;
		for (Entity* e : s->_entities)
			if (e->x() == x && e->y() == y)
				ent.push_back(e);

		return ent;
	}
	else return list<Entity*>();
}

list<pair<pair<int, int>, Entity*>> Sector::entitiesAround(int x, int y,
														   int offX, int offY,
														   int width,
														   int height,
														   bool goNorth,
														   bool goSouth)
{
	// It's probably possible to implement this function in a nicer way,
	// but seriously: who cares
	Sector* s = sectorAt(x, y);
	x = mod(x);
	y = mod(y);

	list<pair<pair<int, int>, Entity*>> ents;

	// sectors to the west
	int tmpX = offX;
	Sector* tmpS = s;
	while (tmpS != nullptr && tmpX + _size >= 0)
	{
		for (Entity* e : tmpS->_entities)
			if (e->x() + tmpX >= 0 && e->y() + offY >= 0 &&
				e->x() + tmpX < width && e->y() + offY < height)
				ents.push_back({{e->x() + tmpX, e->y() + offY}, e});
		tmpX -= _size;
		tmpS = tmpS->_west;
	}

	// sectors to the east
	tmpS = s->_east;
	tmpX = offX + _size;
	while (tmpS != nullptr && tmpX - _size < width)
	{
		for (Entity* e : tmpS->_entities)
			if (e->x() + tmpX >= 0 && e->y() + offY >= 0 &&
				e->x() + tmpX < width && e->y() + offY < height)
				ents.push_back({{e->x() + tmpX, e->y() + offY}, e});
		tmpX += _size;
		tmpS = tmpS->_east;
	}

	// north
	if (goNorth && offY >= 0 && s->_north != nullptr)
		ents.splice(ents.end(),
					s->_north->entitiesAround(x, y, offX, offY - _size,
											  width, height, true, false));

	// north
	if (goSouth && offY - _size < height && s->_south != nullptr)
		ents.splice(ents.end(),
					s->_south->entitiesAround(x, y, offX, offY + _size,
											  width, height, true, false));

	return ents;
}


bool Sector::explored(int x, int y)
{
	Sector* s = sectorAt(x, y);

	if (s != nullptr)
	{
		x = mod(x);
		y = mod(y);
		return s->_explored.at(x + y * _size);
	}
	else
		return false;	// no such tile
}

void Sector::setExplored(int x, int y, bool explored)
{
	Sector* s = sectorAt(x, y);

	if (s != nullptr)
	{
		x = mod(x);
		y = mod(y);
		s->_explored.at(x + y * _size) = explored;
	}
}

Tile* Sector::at(int x, int y)
{
	Sector* s = sectorAt(x, y);

	if (s != nullptr)
	{
		x = mod(x);
		y = mod(y);
		return s->_tiles.at(x + y * _size);
	}
	else
		return nullptr;
}

void Sector::setAt(int x, int y, Tile* tile)
{
	Sector* s = sectorAt(x, y);

	if (s != nullptr)
	{
		x = mod(x);
		y = mod(y);
		_tiles.at(x + y * _size) = tile;
	}
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

void Sector::setStatusBar(StatusBar* statusBar)
{
	_statusBar = statusBar;
}

StatusBar* Sector::statusBar()
{
	return _statusBar;
}

void Sector::setEast(Sector* east)
{
	_east = east;
}

// I'll probably need those for the generator later
//void Sector::hLine(int x, int y, int len, Tile* tile)
//{
//	for (int col = x; col < x + len; col++)
//		setAt(col, y, tile);
//}

//void Sector::vLine(int x, int y, int len, Tile* tile)
//{
//	for (int row = y; row < y + len; row++)
//		setAt(x, row, tile);
//}
//void Sector::square(int x, int y,
//					int width, int height,
//					Tile* tile)
//{
//	for (int row = y; row < y + height; row++)
//		for (int col = x; col < x + width; col++)
//			setAt(col, row, tile);
	
//}

//void Sector::createRoom(int x, int y,
//						int width, int height,
//						Tile* ground, Tile* hWall,
//						Tile* vWall)
//{
//	// north wall
//	hLine(x, y, width, hWall);
//	// south wall
//	hLine(x, y + height - 1, width, hWall);

//	// west wall
//	vLine(x, y + 1, height - 2, vWall);
//	// east wall
//	vLine(x + width - 1, y + 1, height - 2, vWall);

//	// ground
//	square(x + 1, y + 1, width - 2, height - 2, ground);
//}
