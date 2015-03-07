#ifndef ENTITY_H
#define ENTITY_H

#include "sector.h"
#include "tile.h"

class Entity
{
protected:
	const Tile&	_t;
	int			_x;
	int			_y;
	Sector*		_sector;

public:
	Entity(const Tile& t, int x, int y, Sector* sector = nullptr);

	const Tile&	t()			const;
	int			x()			const;
	int			y()			const;
	Sector*		sector()	const;

	void setX(int x);
	void setY(int y);
	void setSector(Sector* Sector);
};
#endif
