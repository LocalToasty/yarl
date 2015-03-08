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
	return _explored.at(x + _width * y);
}

void Sector::setExplored(int x, int y, bool explored)
{
	_explored.at(x + y * _width) = explored;
}

Tile*& Sector::at(int x, int y)
{
	// the tiles are stored linearly
	return _tiles.at(x + width() * y);
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
		at(col, y) = tile;
}

void Sector::vLine(int x, int y, int len, Tile* tile)
{
	for (int row = y; row < y + len; row++)
		at(x, row) = tile;
}
void Sector::square(int x, int y,
					int width, int height,
					Tile* tile)
{
	for (int row = y; row < y + height; row++)
		for (int col = x; col < x + width; col++)
			at(col, row) = tile;
	
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
