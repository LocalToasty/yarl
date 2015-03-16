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

#include "sector.h"

#include "entity.h"
#include <algorithm>
#include <tuple>
#include <cmath>

using namespace std;

const int Sector::_size = 0x40;

StatusBar* Sector::_statusBar = nullptr;

int min(int x, int y)
{
	return (x < y) ? x : y;
}

int max(int x, int y)
{
	return (x > y) ? x : y;
}

int heuristic(int x1, int y1, int x2, int y2)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);

	return max(dx, dy);
}

vector<Command> Sector::path(int x1, int y1, int x2, int y2)
{
	// A* search (see http://en.wikipedia.org/wiki/A*_search_algorithm)

	typedef tuple<Sector*, int, int> C;	// c

	typedef struct Node
	{
		int h;	// heuristic of distance to goal
		int g;	// cost so far
		Command action;	// action taken to reach this node
		Node* parent;	// previous node; nullptr if none
		C c;	// coordinates
	} Node;

	// if the destination is not passable, there is no route to it.
	if (!passableAt(x2, y2))
	{
		return {Command::none};
	}

	// maximum length of path
	int maxLength = heuristic(x1, y1, x2, y2) * 4;

	// frontier is a priority queue initialised with the starting point
	list<Node*> frontier;

	frontier.push_back(new Node {0, 0, Command::none, nullptr,
								 C(sectorAt(x1, y1), x1, y1)});

	vector<Node*> explored;

	C goal(sectorAt(x2, y2), x2, y2);

	for (;;)	// forever
	{
		if (frontier.empty())	// failure
		{
			for (Node* n : explored)	// cleanup
				delete n;

			return {Command::none};
		}

		Node* node = frontier.front();
		frontier.pop_front();

		if (node->c == goal)	// optimal path found
		{
			vector<Command> directions;

			// get all commands
			for (; node->parent != nullptr; node = node->parent)
			{
				directions.push_back(node->action);
			}

			// bring them into the right order
			reverse(directions.begin(), directions.end());

			// cleanup
			for (Node* n : frontier)
				delete n;
			for (Node* n : explored)
				delete n;

			return directions;
		}

		explored.push_back(node);

		// get all the adjacent nodes
		// I hope this wall of text can somehow be broken down...
		vector<pair<Command, C>> passableNeighbours;
		int x = get<1>(node->c);
		int y = get<2>(node->c);

		// north
		int ty = y - 1;
		if (passableAt(x, ty))
			passableNeighbours.push_back({Command::north,
										  C(sectorAt(x, ty), x, ty)});

		int tx = x - 1;
		// north west
		if (passableAt(tx, ty))
			passableNeighbours.push_back({Command::northWest,
										  C(sectorAt(tx, ty), tx, ty)});

		// west
		if (passableAt(tx, y))
			passableNeighbours.push_back({Command::west,
										  C(sectorAt(tx, y), tx, y)});

		ty = y + 1;
		// south west
		if (passableAt(tx, ty))
			passableNeighbours.push_back({Command::southWest,
										  C(sectorAt(tx, ty), tx, ty)});

		// south
		if (passableAt(x, ty))
			passableNeighbours.push_back({Command::south,
										  C(sectorAt(x, ty), x, ty)});

		tx = x + 1;
		// south east
		if (passableAt(tx, ty))
			passableNeighbours.push_back({Command::southEast,
										  C(sectorAt(tx, ty), tx, ty)});

		// east
		if (passableAt(tx, y))
			passableNeighbours.push_back({Command::east,
										  C(sectorAt(tx, y), tx, y)});

		ty = y - 1;
		// north east
		if (passableAt(tx, ty))
			passableNeighbours.push_back({Command::northEast,
										  C(sectorAt(tx, ty), tx, ty)});

		for (pair<Command, C> neighbour : passableNeighbours)
		{
			// if the node is in the explored set, dismiss it
			bool inExplored = false;
			for (Node* n : explored)
			{
				if (n->c == neighbour.second)
				{
					inExplored = true;
					break;
				}
			}

			auto comp = [](const Node* l, const Node* r)
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
					if (n->g >
						node->g + 1 + heuristic(get<1>(neighbour.second),
												get<2>(neighbour.second),
												x2, y2))
					{
						// replace the old node
						delete n;
						frontier.remove(n);

						n = new Node {node->g + 1 +
									  heuristic(get<1>(neighbour.second),
												get<2>(neighbour.second),
												x2, y2),
									  node->g + 1, neighbour.first, node,
									  neighbour.second};

						auto pos = lower_bound(frontier.begin(), frontier.end(),
											   n, comp);

						frontier.insert(pos, n);
						break;
					}
				}
			}

			// if the node is not yet in the frontier and nor has been found
			// yet, add it to the frontier
			if (!inExplored && !inFrontier)
			{
				Node* n = new Node {node->g + 1 +
									heuristic(get<1>(neighbour.second),
											  get<2>(neighbour.second),
											  x2, y2),
											  node->g + 1, neighbour.first, node,
									neighbour.second};

				if (n->g < maxLength)
				{
					auto pos = lower_bound(frontier.begin(), frontier.end(),
										   n, comp);

					frontier.insert(pos, n);
				}
			}
		}
	}
}

Sector* Sector::sectorAt(int x, int y)
{
	if (x >= 0 && y >= 0 && x < _size && y < _size)
	{
		return this;
	}
	// if the x / y coordinates point into an adjacent sector
	else if (x < 0 && _west != nullptr)
	{
		return _west->sectorAt(x + _size , y);
	}
	else if (y < 0 && _north != nullptr)
	{
		return _north->sectorAt(x, y + _size);
	}
	else if (x >= _size && _east != nullptr)
	{
		return _east->sectorAt(x - _size, y);
	}
	else if (y >= _size && _south != nullptr)
	{
		return _south->sectorAt(x, y - _size);
	}
	else
		return nullptr;
}

int Sector::mod(int i)
{
	i %= _size;

	return (i >= 0) ? i : _size + i;
}

Sector::Sector(Tile* defTile, Sector* north, Sector* south,
			   Sector* west, Sector* east) :
	_tiles(_size * _size, defTile), _explored(_size * _size),
	_north(north), _south(south), _west(west), _east(east)
{
}

Sector::~Sector()
{
	for (Entity* e : _entities)
		delete e;
}

// returns true if a line of sight exists between the given points
bool Sector::los(int x1, int y1, int x2, int y2, double max)
{
	if(max > 0 && sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)) > max)
		return false;

	const int dx = x2 - x1;
	const int dy = y2 - y1;

	const int dirX = (dx > 0) ? 1 : -1;
	const int dirY = (dy > 0) ? 1 : -1;

	// coordinates are normalised to the sector of (x1,x2)
	Sector* s = sectorAt(x1, y1);
	x1 = mod(x1);
	y1 = mod(y1);
	x2 = x1 + dx;
	y2 = y1 + dy;

	if (abs(dx) > abs(dy))
	{
		int y = y1;
		int c = abs(dy);

		for (int x = x1; x != x2; x += dirX)
		{
			if (c > abs(dx))
			{
				y += dirY;
				c -= abs(dx);
			}

			// check if LOS is broken
			if (!s->at(x, y)->transparent())
				return false;

			// check if an entity blocks the Line of sight
			for ( Entity* e : s->entitiesAt( x, y ) )
				if ( !e->t().transparent() )
					return false;

			c += abs(dy);
		}
	}
	else
	{
		int x = x1;
		int c = abs(dx);

		for (int y = y1; y != y2; y += dirY)
		{
			if (c > abs(dy))
			{
				x += dirX;
				c -= abs(dy);
			}

			// check if LOS is broken
			if (!s->at(x, y)->transparent())
				return false;

			// check if an entity blocks the Line of sight
			for ( Entity* e : s->entitiesAt( x, y ) )
				if ( !e->t().transparent() )
					return false;

			c += abs(dx);
		}
	}

	return true;
}

// returns true if neither the terrain nor the any entities at the coordinates
// are implassable.
bool Sector::passableAt(int x, int y)
{
	Sector* s = sectorAt(x, y);
	// if the sector at (x1,y1) doesn't exist, the terrain is impassable
	if (s == nullptr)
		return false;

	x = mod(x);
	y = mod(y);

	// check for terrain passability
	if (!s->at(x, y)->passable())
		return false;

	// check for entitiy passability
	for (Entity* e : s->entitiesAt(x, y))
		if (!e->t().passable())
			return false;

	return true;
}

int Sector::size()
{
	return _size;
}

const vector<Tile*>& Sector::tiles()
{
	return _tiles;
}

const list<Entity*>& Sector::entities() const
{
	return _entities;
}

void Sector::addEntity(Entity* e)
{
	// Entities which are passable should be drawn before those wich are not.
	// Entities which are transparent should be drawn before those wich are not.
	// This is to ensure that the impassable / opaque entities are drawn on
	// top.
	auto comp = [](Entity* l, Entity* r)
	{
		return l->t().passable() && !r->t().passable() ||
			   l->t().transparent() && !r->t().transparent();
	};

	auto pos = lower_bound(_entities.begin(), _entities.end(), e, comp);

	_entities.insert(pos, e);
}

void Sector::removeEntity(Entity* e)
{
	_entities.remove(e);
}

list<Entity*> Sector::entitiesAt(int x, int y)
{
	Sector* s = sectorAt(x, y);
	x = mod(x);
	y = mod(y);

	// if tile is in this sector
	if (s != nullptr)
	{
		list<Entity*> ent;
		for (Entity* e : s->_entities)
			if (e->x() == x && e->y() == y)
				ent.push_back(e);

		return ent;
	}
	else return list<Entity*>();
}

list<pair<pair<int, int>, Entity*>> Sector::entitiesAround(int x, int y,
														   int offX, int offY,
														   int width,
														   int height,
														   bool goNorth,
														   bool goSouth)
{
	// It's probably possible to implement this function in a nicer way,
	// but seriously: who cares
	Sector* s = sectorAt(x, y);
	x = mod(x);
	y = mod(y);

	list<pair<pair<int, int>, Entity*>> ents;

	// sectors to the west
	int tmpX = offX;
	Sector* tmpS = s;
	while (tmpS != nullptr && tmpX + _size >= 0)
	{
		for (Entity* e : tmpS->_entities)
			if (e->x() + tmpX >= 0 && e->y() + offY >= 0 &&
				e->x() + tmpX < width && e->y() + offY < height)
				ents.push_back({{e->x() + tmpX, e->y() + offY}, e});
		tmpX -= _size;
		tmpS = tmpS->_west;
	}

	// sectors to the east
	tmpS = s->_east;
	tmpX = offX + _size;
	while (tmpS != nullptr && tmpX - _size < width)
	{
		for (Entity* e : tmpS->_entities)
			if (e->x() + tmpX >= 0 && e->y() + offY >= 0 &&
				e->x() + tmpX < width && e->y() + offY < height)
				ents.push_back({{e->x() + tmpX, e->y() + offY}, e});
		tmpX += _size;
		tmpS = tmpS->_east;
	}

	// north
	if (goNorth && offY >= 0 && s->_north != nullptr)
		ents.splice(ents.end(),
					s->_north->entitiesAround(x, y, offX, offY - _size,
											  width, height, true, false));

	// north
	if (goSouth && offY - _size < height && s->_south != nullptr)
		ents.splice(ents.end(),
					s->_south->entitiesAround(x, y, offX, offY + _size,
											  width, height, true, false));

	return ents;
}


bool Sector::explored(int x, int y)
{
	Sector* s = sectorAt(x, y);

	if (s != nullptr)
	{
		x = mod(x);
		y = mod(y);
		return s->_explored.at(x + y * _size);
	}
	else
		return false;	// no such tile
}

void Sector::setExplored(int x, int y, bool explored)
{
	Sector* s = sectorAt(x, y);

	if (s != nullptr)
	{
		x = mod(x);
		y = mod(y);
		s->_explored.at(x + y * _size) = explored;
	}
}

Tile* Sector::at(int x, int y)
{
	Sector* s = sectorAt(x, y);

	if (s != nullptr)
	{
		x = mod(x);
		y = mod(y);
		return s->_tiles.at(x + y * _size);
	}
	else
		return nullptr;
}

void Sector::setAt(int x, int y, Tile* tile)
{
	Sector* s = sectorAt(x, y);

	if (s != nullptr)
	{
		x = mod(x);
		y = mod(y);
		_tiles.at(x + y * _size) = tile;
	}
}

Sector* Sector::north() const
{
	return _north;
}

void Sector::setNorth(Sector* north)
{
	_north = north;
}

Sector* Sector::south() const
{
	return _south;
}

void Sector::setSouth(Sector* south)
{
	_south = south;
}

Sector* Sector::west() const
{
	return _west;
}

void Sector::setWest(Sector* west)
{
	_west = west;
}

Sector* Sector::east() const
{
	return _east;
}

void Sector::setStatusBar(StatusBar* statusBar)
{
	_statusBar = statusBar;
}

StatusBar* Sector::statusBar()
{
	return _statusBar;
}

void Sector::setEast(Sector* east)
{
	_east = east;
}

// I'll probably need those for the generator later
//void Sector::hLine(int x, int y, int len, Tile* tile)
//{
//	for (int col = x; col < x + len; col++)
//		setAt(col, y, tile);
//}

//void Sector::vLine(int x, int y, int len, Tile* tile)
//{
//	for (int row = y; row < y + len; row++)
//		setAt(x, row, tile);
//}
//void Sector::square(int x, int y,
//					int width, int height,
//					Tile* tile)
//{
//	for (int row = y; row < y + height; row++)
//		for (int col = x; col < x + width; col++)
//			setAt(col, row, tile);

//}

//void Sector::createRoom(int x, int y,
//						int width, int height,
//						Tile* ground, Tile* hWall,
//						Tile* vWall)
//{
//	// north wall
//	hLine(x, y, width, hWall);
//	// south wall
//	hLine(x, y + height - 1, width, hWall);

//	// west wall
//	vLine(x, y + 1, height - 2, vWall);
//	// east wall
//	vLine(x + width - 1, y + 1, height - 2, vWall);

//	// ground
//	square(x + 1, y + 1, width - 2, height - 2, ground);
//}
