#ifndef YARL_H
#define YARL_H

#include "globals.h"
#include "sector.h"
#include "character.h"
#include "tile.h"

class Yarl
{
private:
	Sector*		_currentSector;
	Character*	_player;

	bool		_color;

	// tiles the example room will be made off
	static const Tile none;
	static const Tile ground;
	static const Tile wallNS;
	static const Tile wallWE;
	static const Tile corridor;


	// representation of the player character
	static const Tile player;

private:
	bool init();

	void render();
	bool loop();

	void cleanup();

public:
	Yarl(int argc, char* argv[]);

	int exec();
};

#endif
