#ifndef TILE_H
#define TILE_H

#include <string>
#include <curses.h>

using namespace std;

class Tile
{
private:
	char	_repr;
	short	_color;
	string	_description;
	bool	_opaque;
	bool	_passable;

public:
	Tile(char repr = ' ', short color = COLOR_BLACK, string description = "",
		 bool opaque = false);
	Tile(char repr, short color, string description, bool opaque,
		 bool passable);

	char	repr()			const;
	short	color()			const;
	string	description()	const;
	bool	passable()		const;
	bool	opaque()		const;

	void	setRepr(char repr);
	void	setColor(short color);
	void	setDescription(string description);
	void	setPassable(bool passable);
	void	setOpaque(bool opaque);
};

#endif
