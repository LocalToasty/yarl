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

	// tiles the example room will be made off
	static Tile none;
	static Tile ground;
	static Tile wallNS;
	static Tile wallWE;
	static Tile tree;
	static Tile corridor;


	// representation of the player character
	static Tile player;

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
