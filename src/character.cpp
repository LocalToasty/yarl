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
		if (sector()->passableAt(x() - 1, y()))
			setX(x() - 1);
		break;

	case Command::east:
		if (sector()->passableAt(x() + 1, y()))
			setX(x() + 1);
		break;

	case Command::north:
		if(sector()->passableAt(x(), y() - 1))
			setY(y() - 1);
		break;

	case Command::south:
		if (sector()->passableAt(x(), y() + 1))
			setY(y() + 1);
		break;

	case Command::northWest:
		if (sector()->passableAt(x() - 1, y() - 1))
		{
			setX(x() - 1);
			setY(y() - 1);
		}
		break;

	case Command::northEast:
		if (sector()->passableAt(x() + 1, y() - 1))
		{
			setX(x() + 1);
			setY(y() - 1);
		}
		break;

	case Command::southWest:
		if (sector()->passableAt(x() - 1, y() + 1))
		{
			setX(x() - 1);
			setY(y() + 1);
		}
		break;

	case Command::southEast:
		if (sector()->passableAt(x() + 1, y() + 1))
		{
			setX(x() + 1);
			setY(y() + 1);
		}
		break;

	}
}

bool Character::los(int x, int y) const
{
	return sector()->los(this->x(), this->y(), x, y);
}

bool Character::los(Entity* other) const
{
	return sector()->los(x(), y(), other->x(), other->y());
}
