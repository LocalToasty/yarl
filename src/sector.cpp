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

const int Sector::_width	= 0x40;
const int Sector::_height	= 0x40;

// Returns the sector in which the given coordinates lie and modifies the
// coordinates accordingly.
Sector* Sector::sectorAt(int& x, int& y)
{
	if (x >= 0 && x < _width && y >= 0 && y < _height)
	{
		return this;
	}
	// if the x / y coordinates point into an adjacent sector
	else if (x < 0 && _west != nullptr)
	{
		x += _width;
		return _west->sectorAt(x, y);
	}
	else if (y < 0 && _north != nullptr)
	{
		y += _height;
		return _north->sectorAt(x, y);
	}
	else if (x >= _width && _east != nullptr)
	{
		x -= _width;
		return _east->sectorAt(x, y);
	}
	else if (y >= _height && _south != nullptr)
	{
		y -= _height;
		return _south->sectorAt(x, y);
	}
	else
		return nullptr;
}

Sector::Sector(Tile* defTile, Sector* north, Sector* south,
			   Sector* west, Sector* east) :
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
bool Sector::los(int x1, int y1, int x2, int y2, double max)
{
	if(max > 0 && sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)) > max)
		return false;

	const int dx = x2 - x1;
	const int dy = y2 - y1;

	const int dirX = (dx > 0) ? 1 : -1;
	const int dirY = (dy > 0) ? 1 : -1;

	// list of all entities capable of blocking the line of sight
	list<Entity*> blockingEntities;
	// TODO: optimisation
	for (pair<pair<int, int>, Entity*> e :
		 entitiesAround(x1, x2, abs(dx), abs(dx), abs(dy), abs(dy), 0, 0))
	{
		if (!e.second->t().opaque())
			blockingEntities.push_back(e.second);
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
	Sector* s = sectorAt(x, y);
	if (s == nullptr)
		return false;

	if (!s->at(x, y)->passable())
		return false;

	for (Entity* e : s->entitiesAt(x, y))
		if (!e->t().passable())
			return false;

	return true;
}

int Sector::width()
{
	return _width;
}

int Sector::height()
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

list<Entity*> Sector::entitiesAt(int x, int y)
{
	Sector* s = sectorAt(x, y);

	// if tile is in this sector
	if (s != nullptr)
	{
		list<Entity*> ent;
		for (Entity* e : s->_entities)
			if (e->x() == x && e->y() == y)
				ent.push_back(e);

		int i = ent.size();
		return ent;
	}
	else return list<Entity*>();
}

list<pair<pair<int, int>, Entity*>> Sector::entitiesAround(int x, int y,
														   int rw, int re,
														   int rn, int rs,
														   int offX, int offY)
{
	Sector* s = sectorAt(x, y);
	list<pair<pair<int, int>, Entity*>> ents;

	for (Entity* e : s->_entities)
		if (e->x() >= x - rw && e->x() <= x + re &&
				e->y() >= y - rn && e->y() <= y + rs)
			ents.push_back({{e->x() + offX, e->y() + offY}, e});

	if (rw > x && _west != nullptr)
		ents.splice(ents.end(),
					s->_west->entitiesAround(x, y, rw - _width, _width - x,
											 (rn > y) ? y : rn,
											 _height - y,
											 offX - _width, offY));
	if (re > _width - x && _east != nullptr)
		ents.splice(ents.end(),
					s->_east->entitiesAround(x, y, x, re - _width,
											 rn > y ? y : rn,
											 rs > _height - y ? _height - y
															  : rs,
											 offX + _width, offY));
	if (rn > y && _north != nullptr)
		ents.splice(ents.end(),
					s->_north->entitiesAround(x, y, rw, re,
											  rn - _height, _height - y,
											  offX, offY - _height));
	if (rs > _height - y && _south != nullptr)
		ents.splice(ents.end(),
					s->_south->entitiesAround(x, y, rw, re,
											  y, rs - _height,
											  offX, offY + _height));
	return ents;
}


bool Sector::explored(int x, int y)
{
	Sector* s = sectorAt(x, y);

	if (s != nullptr)
		return s->_explored.at(x + y * _width);
	else
		return false;	// no such tile
}

void Sector::setExplored(int x, int y, bool explored)
{
	Sector* s = sectorAt(x, y);

	if (s != nullptr)
		s->_explored.at(x + y * _width) = explored;
}

Tile* Sector::at(int x, int y)
{
	Sector* s = sectorAt(x, y);

	if (s != nullptr)
		return s->_tiles.at(x + _width * y);
	else
		return nullptr;
}

void Sector::setAt(int x, int y, Tile* tile)
{
	Sector* s = sectorAt(x, y);

	if (s != nullptr)
	{
		_tiles.at(x + _width * y) = tile;
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
