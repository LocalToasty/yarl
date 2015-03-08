#ifndef YARL_H
#define YARL_H

#include "globals.h"
#include "sector.h"
#include "character.h"
#include "tile.h"
#include "variable.h"
#include <string>
#include <map>

using namespace std;

class Yarl
{
private:
	Sector*		_currentSector;
	Character*	_player;

	map<string, Variable>	_variables;

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
