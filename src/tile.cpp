#include "tile.h"


bool Tile::opaque() const
{
	return _opaque;
}

void Tile::setOpaque(bool opaque)
{
	_opaque = opaque;
}
Tile::Tile(char repr, short color, string description, bool opaque) :
	_repr(repr), _color(color), _description(description),
	_opaque(opaque), _passable(opaque)
{}

Tile::Tile(char repr, short color, string description,
		   bool opaque, bool passable) :
	_repr(repr), _color(color), _description(description),
	_opaque(opaque), _passable(passable)
{

}

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
