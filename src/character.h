#ifndef CHARACTER_H
#define CHARACTER_H

#include "globals.h"
#include "entity.h"
#include "tile.h"
#include "sector.h"

class Character : public Entity
{
private:

public:
	Character(const Tile& t, int x, int y, Sector* currentSector = nullptr);

	void move(Command dir);

	bool los(int x, int y)	const;
	bool los(Entity* other)	const;
};

#endif
