#include "sector.h"

Sector::Sector(const int width, const int height, const Tile& defTile) :
	_width(width), _height(height),
	_tiles(vector<Tile>(width * height))
{
	for (Tile& t : _tiles)
		t = defTile;
}

bool Sector::los(int x1, int y1, int x2, int y2)
{

}

int Sector::width() const
{
	return _width;
}

int Sector::height() const
{
	return _height;
}

const vector<Tile>& Sector::tiles()
{
	return _tiles;
}

list<Entity*>& Sector::entities()
{
	return _entities;
}

Tile& Sector::at(int x, int y)
{
	// the tiles are stored linearly
	return _tiles.at(x + width() * y);
}

void Sector::hLine(const int x, const int y, const int len, const Tile& tile)
{
	for (int col = x; col < x + len; col++)
		at(col, y) = tile;
}

void Sector::vLine(const int x, const int y, const int len, const Tile& tile)
{
	for (int row = y; row < y + len; row++)
		at(x, row) = tile;
}
void Sector::square(const int x, const int y,
						  const int width, const int height,
						  const Tile& tile)
{
	for (int row = y; row < y + height; row++)
		for (int col = x; col < x + width; col++)
			at(col, row) = tile;
	
}

void Sector::createRoom(const int x, const int y,
							  const int width, const int height,
							  const Tile& ground, const Tile& hWall, const Tile& vWall)
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
