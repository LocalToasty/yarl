#include "character.h"
#include "sector.h"

Character::Character(const Tile& t, int x, int y, Sector* currentSector) :
	Entity(t, x, y, currentSector)
{
}

void Character::move(Command dir)
{
	switch (dir) {
	case Command::west:
		if (sector()->at(_x - 1, _y).passable())
			_x--;
		break;

	case Command::east:
		if (sector()->at(_x + 1, _y).passable())
			_x++;
		break;

	case Command::north:
		if(sector()->at(_x, _y - 1).passable())
			_y--;
		break;

	case Command::south:
		if (sector()->at(_x, _y + 1).passable())
			_y++;
		break;

	case Command::northWest:
		if (sector()->at(_x - 1, _y - 1).passable())
		{
			_x--;
			_y--;
		}
		break;

	case Command::northEast:
		if (sector()->at(_x + 1, _y - 1).passable())
		{
			_x++;
			_y--;
		}
		break;

	case Command::southWest:
		if (sector()->at(_x - 1, _y + 1).passable())
		{
			_x--;
			_y++;
		}
		break;

	case Command::southEast:
		if (sector()->at(_x + 1, _y + 1).passable())
		{
			_x++;
			_y++;
		}
		break;

	}
}

bool Character::los(int x, int y) const
{
	return _sector->los(_x, _y, x, y);
}

bool Character::los(Entity& other) const
{
	return _sector->los(x(), y(), other.x(), other.y());
}
