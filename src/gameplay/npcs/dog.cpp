#include "dog.h"
#include "world.h"
#include "entity.h"
#include "character.h"

Tile Dog::dog = { 'd', Color::red, "Dog", true, false };

Dog::Dog( int x, int y, World* world )
    : NPC( dog, x, y, 3, 12, world )
{

}

void Dog::think()
{
	for( Entity* e : world()->entities( x() - visionRange(),
										y() - visionRange(),
										x() + visionRange(),
										y() + visionRange() ) )
	{
		Character* c = dynamic_cast<Character*>( e );
		if( c != nullptr && c != this && los( e ) )
		{
			_waypointX = e->x();
			_waypointY = e->y();
			break;
		}
	}

	Command cmd = world()->route( x(), y(),
								  _waypointX, _waypointY, true ).front();

	int dx;
	int dy;

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
