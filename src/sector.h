#ifndef SECTOR_H
#define SECTOR_H

#include "globals.h"
#include "tile.h"
#include <vector>
#include <list>

using namespace std;

class Entity;

class Sector
{
private:
	int				_width;
	int				_height;
	vector<Tile>	_tiles;
	list<Entity*>	_entities;

public:
	Sector(const int width, const int height, const Tile& defTile);

	bool los(int x1, int y1, int x2, int y2);

	int	width()		const;
	int	height()	const;

	const vector<Tile>&	tiles();	// returns all tiles
	list<Entity*>&		entities();
	
	// returns the tile at the given location
	Tile&	at(int x, int y);

	// creates a horizontal / vertical line of tiles
	void hLine(const int x, const int y, const int len, const Tile& tile);
	void vLine(const int x, const int y, const int len, const Tile& tile);

	// creates a square with the given dimensions
	void square(const int x, const int y,
				const int width, const int height,
				const Tile& tile);

	void createRoom(const int x, const int y,
					const int width, const int height,
					const Tile& ground, const Tile& hWall, const Tile& vWall);
};

#endif	// SECTOR_H
