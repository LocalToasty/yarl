/*
 * YARL - Yet another Roguelike
 * Copyright (C) 2015  Marko van Treeck <markovantreeck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "world.h"
#include "sector.h"
#include "character.h"
#include "player.h"
#include "companion.h"
#include "item.h"
#include <cmath>
#include <algorithm>

using namespace std;

Tile World::_none	= {' ', Color::black,	"void"};
Tile World::_mud	= {'.', Color::red,		"mud", true};
Tile World::_grass	= {',', Color::green,	"patch of grass", true};
Tile World::_tree	= {'T', Color::green,	"tree"};
Tile World::_hero	= {'@', Color::yellow,	"you", true, false };
Tile World::_goblin	= { 'g', Color::green,	"goblin", true, false };
Tile World::_dog	= {'d', Color::red,		"dog", true, false };
Tile World::_dogCorpse	= { '%', Color::red,	"dog corpse", true };
Tile World::_shortSword	= { '(', Color::white,	"short sword", true };
Tile World::_leatherArmor	= { '[', Color::red,	"leather armor", true };

World::World( int width, int height ) :
	_width( width ), _height( height ), _sectors( width * height )
{
	for( Sector*& s : _sectors )
	{
		s = new Sector( &_grass );

		for( int x = 0; x < Sector::size(); x++ )
		{
			for( int y = 0; y < Sector::size(); y++ )
			{
				if( rand() % 16 == 0 )
					s->setTile( x, y, &_tree );
				else if( rand() % 8 == 0 )
					s->setTile( x, y, &_mud );
			}
		}
	}

	array<int, 6> attr = { 12, 12, 12, 12, 12, 12 };
	_player = new Player( _hero, 42, 42, 9 + rand() % 8 , 16, attr,
						  new Weapon( {}, [](){ return rand() % 2 + 1; },
									  *this ), *this, {}, 1 );

	Weapon* weap = new Weapon( _shortSword, [](){ return rand() % 6 + 1; },
							   *this );
	_player->inventory().push_back( weap );
	_player->setWeapon( weap );

	Armor* arm = new Armor( _leatherArmor, 2, 6, 4, *this );
	_player->inventory().push_back( arm );
	_player->setArmor( arm );

	attr = { 13, 13, 15, 2, 12, 6 };
	new Companion( _dog, _player, 45, 46, rand() % 8 + 3, 12, attr,
				   new Weapon( {}, [](){ return rand() % 4 + 2; }, *this),
				   *this, { new Item( _dogCorpse, *this, -1, -1, 1,
							Entity::Size::small ) },
				   2, Entity::Size::small, 1 );

	attr = { 11, 15, 12, 10, 9, 6 };
	new Companion( _goblin, nullptr, 45, 45, rand() % 10 + 2, 12, attr,
				   new Weapon( {}, [](){ return rand() % 2 + 1; }, *this ),
				   *this );
}

double World::distance(int x1, int y1, int x2, int y2)
{
	return sqrt( ( x2 - x1 ) * ( x2 - x1 ) + ( y2 - y1 ) * ( y2 - y1 ) );
}

bool World::los( int x1, int y1, int x2, int y2, double range )
{
	if( range > 0 && distance( x1, y1, x2, y2 ) > range )
		return false;

	const int dx = x2 - x1;
	const int dy = y2 - y1;

	const int dirX = ( dx > 0 ) ? 1 : -1;
	const int dirY = ( dy > 0 ) ? 1 : -1;

	vector<Entity*> blocking;
	for( Entity* e :
		 entities( min( x1, x2 ), min( y1, y2 ),
				   max( x1, x2 ) + 1, max( y1, y2 ) + 1 ) )
		if( !e->t().transparent() )
			blocking.push_back( e );

	if ( abs( dx ) > abs( dy ) )	// in x direction
	{
		int y = y1;
		int c = abs( dy );

		for ( int x = x1; x != x2; x += dirX )
		{
			if ( c > abs( dx ) )
			{
				y += dirY;
				c -= abs( dx );
			}

			// check if LOS is broken
			if ( !tile(x, y)->transparent() )
				return false;

			// check if an entity blocks the Line of sight
			for ( Entity* e : blocking )
				if ( e->x() == x && e->y() == y )
					return false;

			c += abs(dy);
		}
	}
	else	// in y direction
	{
		int x = x1;
		int c = abs(dx);

		for ( int y = y1; y != y2; y += dirY )
		{
			if (c > abs(dy))
			{
				x += dirX;
				c -= abs(dy);
			}

			// check if LOS is broken
			if ( !tile( x, y )->transparent() )
				return false;

			for ( Entity* e : blocking )
				if ( e->x() == x && e->y() == y )
					return false;

			c += abs( dx );
		}
	}

	return true;
}


int min(int x, int y)
{
	return (x < y) ? x : y;
}

int max(int x, int y)
{
	return (x > y) ? x : y;
}

int heuristic( int x1, int y1, int x2, int y2 )
{
	int dx = abs( x2 - x1 );
	int dy = abs( y2 - y1 );

	return min( dx, dy ) + 2 * max( dx, dy );
}

// calculates a route from (x1, y2) to (x2, y2). If converge is true, the path
// will only lead to a tile next to the destination.
vector<Command> World::route( int x1, int y1, int x2, int y2 , bool converge )
{
	// A* search (see http://en.wikipedia.org/wiki/A*_search_algorithm)

	typedef struct Node
	{
		int h;	// heuristic of distance to goal
		int g;	// cost so far
		Command action;	// action taken to reach this node
		Node* parent;	// previous node; nullptr if none
		pair<int, int> c;	// coordinates
	} Node;

	// if the destination is not passable, there is no route to it.
	if( !passable( x2, y2 ) && !converge )
	{
		return { Command::none };
	}

	// frontier is a priority queue initialised with the starting point
	list<Node*> frontier;

	frontier.push_back( new Node { 0, 0, Command::none,
								   nullptr, { x1, y1 } } );

	vector<Node*> explored;

	pair<int, int> goal( x2, y2 );

	for( ;; )	// forever
	{
		if( frontier.empty() )	// failure
		{
			for( Node* n : explored )	// cleanup
				delete n;

			return { Command::none };
		}

		Node* node = frontier.front();
		frontier.pop_front();

		if ( node->c == goal )	// optimal path found
		{
			vector<Command> directions;

			// get all commands
			for ( ; node->parent != nullptr; node = node->parent )
			{
				directions.push_back( node->action );
			}

			// bring them into the right order
			reverse( directions.begin(), directions.end() );

			// cleanup
			for ( Node* n : frontier )
				delete n;
			for ( Node* n : explored )
				delete n;

			if( directions.empty() )
				return { Command::none };

			return directions;
		}

		explored.push_back( node );

		// get all the adjacent nodes
		// I hope this wall of text can somehow be broken down...
		vector<pair<Command, pair<int, int>>> passableNeighbours;
		int x = node->c.first;
		int y = node->c.second;

		// north
		int ty = y - 1;
		if( passable( x, ty ) || ( converge && x == x2 && ty == y2 ) )
			passableNeighbours.push_back( { Command::north, { x, ty } } );

		int tx = x - 1;
		// north west
		if( passable( tx, ty ) || ( converge && tx == x2 && ty == y2 ) )
			passableNeighbours.push_back( { Command::northWest, { tx, ty } } );

		// west
		if ( passable( tx, y ) || ( converge && tx == x2 && y == y2 ) )
			passableNeighbours.push_back( { Command::west, { tx, y } } );

		ty = y + 1;
		// south west
		if( passable( tx, ty ) || ( converge && tx == x2 && ty == y2 ) )
			passableNeighbours.push_back( { Command::southWest, { tx, ty } } );

		// south
		if( passable( x, ty ) || ( converge && x == x2 && ty == y2 ) )
			passableNeighbours.push_back( { Command::south, { x, ty } } );

		tx = x + 1;
		// south east
		if( passable( tx, ty ) || ( converge && tx == x2 && ty == y2 ) )
			passableNeighbours.push_back( { Command::southEast, { tx, ty } } );

		// east
		if ( passable( tx, y ) || ( converge && tx == x2 && y == y2 ) )
			passableNeighbours.push_back( { Command::east, { tx, y } } );

		ty = y - 1;
		// north east
		if ( passable(tx, ty) || ( converge && tx == x2 && ty == y2 ) )
			passableNeighbours.push_back({ Command::northEast, { tx, ty } } );

		for ( pair<Command, pair<int, int>> neighbour : passableNeighbours )
		{
			// if the node is in the explored set, dismiss it
			bool inExplored = false;
			for ( Node* n : explored )
			{
				if ( n->c == neighbour.second )
				{
					inExplored = true;
					break;
				}
			}

			int movementCost = node->g;

			switch( neighbour.first )
			{
			case Command::north:
			case Command::south:
			case Command::west:
			case Command::east:
				movementCost = 2;
				break;

			case Command::northWest:
			case Command::northEast:
			case Command::southWest:
			case Command::southEast:
				movementCost = 3;
				break;
			}

			int h = heuristic( neighbour.second.first,
							   neighbour.second.second, x2, y2 ) +
					movementCost;

			auto comp = []( const Node* l, const Node* r )
			{
				return l->h < r->h;
			};

			bool inFrontier = false;
			for (Node* n : frontier)
			{

				if (n->c == neighbour.second)
				{
					inFrontier = true;

					// node was already found earlier, but the newly found path
					// to node is shorter
					if ( n->g > movementCost )
					{
						// replace the old node
						delete n;
						frontier.remove(n);

						n = new Node { h, movementCost, neighbour.first, node,
									   neighbour.second };

						auto pos = lower_bound( frontier.begin(),
												frontier.end(),
												n, comp);

						frontier.insert( pos, n );
						break;
					}
				}
			}

			// if the node is not yet in the frontier and nor has been found
			// yet, add it to the frontier
			if ( !inExplored && !inFrontier )
			{
				Node* n = new Node { h, movementCost, neighbour.first, node,
									 neighbour.second } ;

				auto pos = lower_bound(frontier.begin(), frontier.end(),
										   n, comp);

				frontier.insert(pos, n);
			}
		}
	}
}

Sector* World::sector( int x, int y )
{
	if( x >= 0 && y >= 0 &&
		x < _width * Sector::size() && y < _height * Sector::size() )
		return _sectors.at( x / Sector::size() + y / Sector::size() * _width );
	else
		return nullptr;
}

Character* World::player()
{
	return _player;
}

Tile* World::tile( int x, int y )
{
	Sector* s = sector( x, y );

	if( s )
		return s->tile( x, y );
	else
		return nullptr;
}

void World::setTile(int x, int y, Tile* t)
{
	Sector* s = sector( x, y );

	if( s != nullptr )
		return s->setTile( x, y, t );
}

bool World::explored(int x, int y)
{
	Sector* s = sector( x, y );

	if( s )
		return s->explored( x, y );
	else
		return nullptr;
}

void World::setExplored( int x, int y, bool explored )
{
	Sector* s = sector( x, y );

	if( s != nullptr )
		s->setExplored( x, y, explored );
}

bool World::passable(int x, int y)
{
	Sector* s = sector( x, y );

	if( s != nullptr )
		return s->passable( x, y );
	else
		return false;
}

vector<Entity*> World::entities( int x, int y )
{
	Sector* s = sector( x, y );

	if( s != nullptr )
		return s->entities( x, y );
	else
		return vector<Entity*>();
}

vector<Entity*> World::entities( int x1, int y1, int x2, int y2 )
{
	vector<Entity*> ents;

	for( int x = x1; x < x2 + Sector::size(); x += Sector::size() )
	{
		for( int y = y1; y < y2 + Sector::size(); y += Sector::size() )
		{
			Sector* s = sector( x, y );

			if( s != nullptr )
				for( Entity* e : s->entities() )
					if( e->x() >= x1 && e->x() < x2 &&
						e->y() >= y1 && e->y() < y2 )
						ents.push_back( e );
		}
	}

	return ents;
}

void World::addEntitiy( Entity* e )
{
	Sector* s = sector( e->x(), e->y() );

	if( s != nullptr )
		s->addEntity( e );
}

void World::removeEntity( Entity* e )
{
	if( e->sector() != nullptr )
		e->sector()->removeEntity( e );
}

double World::time()
{
	return _time;
}

void World::letTimePass(double time)
{
	_time += time;
}

void World::think()
{
	for( Entity* e : entities( _player->x() - Sector::size(),
							   _player->y() - Sector::size(),
							   _player->x() + Sector::size(),
							   _player->y() + Sector::size() ) )
		if( e->hp() > 0 )	// only let living entities think
			e->think();
}

StatusBar& World::statusBar()
{
	return _statusBar;
}
