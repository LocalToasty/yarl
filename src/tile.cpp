#include "tile.h"

Tile::Tile(char repr, short color, string description, bool passable) :
	_repr(repr), _color(color), _passable(passable), _description(description)
{}

char Tile::repr() const
{
	return _repr;
}

short Tile::color() const
{
	return _color;
}

bool Tile::passable() const
{
	return _passable;
}

void Tile::setRepr(char repr)
{
	_repr = repr;
}

void Tile::setColor(short color)
{
	_color = color;
}

void Tile::setPassable(bool passable)
{
	_passable = passable;
}
