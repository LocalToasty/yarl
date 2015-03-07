#include "character.h"
#include "sector.h"

Character::Character(const Tile& t, int x, int y, Sector* currentSector) :
	Entity(t, x, y, currentSector)
{
}

const void Character::move(Dir dir)
{
	switch (dir) {
	case DIR_W:
		if (sector()->at(_x - 1, _y).passable())
			_x--;
		break;

	case DIR_E:
		if (sector()->at(_x + 1, _y).passable())
			_x++;
		break;

	case DIR_N:
		if(sector()->at(_x, _y - 1).passable())
			_y--;
		break;

	case DIR_S:
		if (sector()->at(_x, _y + 1).passable())
			_y++;
		break;
	}
}
