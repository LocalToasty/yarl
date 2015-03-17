#ifndef NPC_H
#define NPC_H

#include "character.h"
#include "command.h"

class NPC : public Character
{
public:
    NPC( const Tile& t, int x, int y, int hp, int visionRange,
		 World* world, const list<Item*>& inventory = {} );

	virtual void think() = 0;
};

#endif // NPC_H
