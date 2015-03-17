#ifndef DOG_H
#define DOG_H

#include "npc.h"

class World;

class Dog : public NPC
{
private:
	int _waypointX { -1 };
	int _waypointY { -1 };

    static Tile dog;

public:
	Dog( int x, int y, World* world );

	void think();
};

#endif
