#ifndef CHARACTER_H
#define CHARACTER_H

#include "globals.h"
#include "entity.h"
#include "tile.h"
#include "sector.h"

enum Dir
{
	DIR_N	= 'k',
	DIR_S	= 'j',
	DIR_W	= 'h',
	DIR_E	= 'l',
	DIR_NW	= 'y',
	DIR_NE	= 'u',
	DIR_SW	= 'b',
	DIR_SE	= 'n'
};

class Character : public Entity
{
private:

public:
	Character(const Tile& t, int x, int y, Sector* currentSector = nullptr);

	const void move(Dir dir);
};

#endif
