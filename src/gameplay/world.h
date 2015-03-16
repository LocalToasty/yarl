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

#ifndef WORLD_H
#define WORLD_H

#include "command.h"
#include "statusbar.h"
#include <vector>

using namespace std;

class Sector;
class Character;
class Tile;
class Entity;

class World
{
private:
	int _width;
	int _height;

	vector<Sector*> _sectors;

	StatusBar _statusBar;

	Character* _player;

	static Tile grass;
	static Tile mud;
	static Tile tree;
	static Tile none;
	static Tile bush;
	static Tile stump;
	static Tile hero;

public:
	World( int width, int height );
	~World();

	bool los(int x1, int y1, int x2, int y2, double range = -1 );
	vector<Command> route( int x1, int y1, int x2, int y2 );

	Sector* sector( int x, int y );
	Character* player();

	Tile* tile( int x, int y );
	void setTile( int x, int y, Tile* t );

	bool explored( int x, int y );
	bool setExplored( int x, int y, bool explored = true );

	bool passable( int x, int y );

	vector<Entity*> entities( int x, int y );
	vector<Entity*> entities( int x1, int y1, int x2, int y2 );
	void addEntitiy( Entity* e );
	void removeEntity( Entity* e );

	StatusBar& statusBar();
};

#endif
