#include "npc.h"
#include "character.h"

NPC::NPC( const Tile& t, int x, int y, int hp, int visionRange,
          World* world, const list<Item*>& inventory ) :
    Character( t, x, y, hp, visionRange, world, inventory )
{
}

