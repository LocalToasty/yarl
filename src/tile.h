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
	bool	_passable;
	string	_description;

public:
	Tile(char repr = 0, short color = COLOR_WHITE, string description = "", bool passable = false);

	char	repr()			const;
	short	color()			const;
	bool	passable()		const;
	string	description()	const;

	void	setRepr(char repr);
	void	setColor(short color);
	void	setPassable(bool passable);
	void	setDescription(string description);
};

#endif
