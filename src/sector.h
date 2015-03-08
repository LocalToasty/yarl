#ifndef SECTOR_H
#define SECTOR_H

#include "globals.h"
#include "tile.h"
#include <vector>
#include <list>

using namespace std;

// declaration to not run into recursion issiues
class Entity;

class Sector
{
private:
	// size of a sector has to be hardwired so they ca be tiled
	static const int _width;
	static const int _height;

private:
	// vector containing the tiles (stored linearly row for row)
	vector<Tile>	_tiles;
	vector<bool>	_explored;

	// a list of all entities in the sector (i.e. characters, items, props)
	list<Entity*>	_entities;

	// pointer to adjacent sectors
	Sector*			_north;
	Sector*			_south;
	Sector*			_west;
	Sector*			_east;

public:
	Sector(const Tile& defTile, Sector* north = nullptr, Sector* south = nullptr, Sector* west = nullptr, Sector* east = nullptr);

	bool			los(int x1, int y1, int x2, int y2);
	list<Command>	route(int x1, int y1, int x2, int y2);

	int	width()		const;
	int	height()	const;

	const vector<Tile>&	tiles();	// returns all tiles
	list<Entity*>&		entities();

	bool	explored(int x, int y);
	void	setExplored(int x, int y, bool explored = true);

	// getters and setters for adjacent sectors
	Sector*	north()	const;
	Sector*	south()	const;
	Sector*	west()	const;
	Sector*	east()	const;

	void	setNorth(Sector* north);
	void	setSouth(Sector* south);
	void	setWest(Sector* west);
	void	setEast(Sector* east);
	
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
