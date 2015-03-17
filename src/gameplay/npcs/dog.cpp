#include "dog.h"
#include "world.h"
#include "entity.h"
#include "character.h"
#include <cstdlib>

Tile Dog::dog = { 'd', Color::red, "Dog", true, false };

Dog::Dog( int x, int y, World* world )
    : NPC( dog, x, y, 3, 12, world )
{

}

void Dog::think()
{
	for( Entity* e : world()->entities( x() - visionRange(),
										y() - visionRange(),
										x() + visionRange() + 1,
										y() + visionRange() + 1 ) )
	{
		Character* c = dynamic_cast<Character*>( e );
		if( c != nullptr && c != this && los( e ) )
		{
			_waypointX = e->x() + ( rand() % 9 ) - 4;
			_waypointY = e->y() + ( rand() % 9 ) - 4;
			break;
		}
	}

	Command cmd = world()->route( x(), y(),
								  _waypointX, _waypointY, true ).front();

	int dx = 0;
	int dy = 0;

	if( cmd == Command::west ||
		cmd == Command::northWest ||
		cmd == Command::southWest )
		dx = -1;
	else if( cmd == Command::east ||
			 cmd == Command::northEast ||
			 cmd == Command::southEast )
		dx = 1;

	if( cmd == Command::north ||
		cmd == Command::northWest ||
		cmd == Command::northEast )
		dy = -1;
	else if( cmd == Command::south ||
			 cmd == Command::southWest ||
			 cmd == Command::southEast )
		dy = 1;

	move( dx, dy );
}
