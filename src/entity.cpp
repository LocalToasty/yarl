#include "entity.h"

Entity::Entity(const Tile& t, int x, int y, Sector* sector) :
	_t(t), _x(x), _y(y), _sector(sector)
{
}

const Tile& Entity::t() const
{
	return _t;
}

int Entity::x() const
{
	return _x;
}

int Entity::y() const
{
	return _y;
}

Sector* Entity::sector() const
{
	return _sector;
}

void Entity::setX(int x)
{
	if (x > 0 && x < _sector->width())
		_x = x;
}

void Entity::setY(int y)
{
	if (y > 0 && y < _sector->height())
		_y = y;
}

void Entity::setSector(Sector* sector)
{
	_sector = sector;
}
