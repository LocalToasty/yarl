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

#ifndef YARL_H
#define YARL_H

#include "sector.h"
#include "tile.h"
#include "variable.h"
#include "statusbar.h"
#include "command.h"
#include <iostream>
#include <string>
#include <map>

using namespace std;

class World;
class Player;
class IOManager;

class Yarl
{
private:
	World* _world;

	IOManager* _iom;

	map<string, Variable> _variables;
	map<char, Command> _bindings;

	enum class State
	{
		def,
		moreMessages,
		showInventory,
		drop,
		equip,
		equip_selectHand,
		examine,
		unequip
	};
	State _state { State::def };

	// some buffer variables
	string _buf;
	int _x, _y;

	void world_render( Player* player, int width, int height );
	void inventory_render();
	void drop_render( Player* player );
	void equip_render( Player* player );
	void unequip_render( Player* player );
	void examine_render();
	void charInfo_render( Player* player, int width, int height );

	void drop_logic( char input, Player* player );
	void equip_logic( char input, Player* player );
	void equip_selectHand_logic( char input, Player* player );
	void unequip_logic( char input, Player* player );
	void examine_logic( char input, Command cmd, Player* player );
	void move_logic( char input, Command cmd, Player* player );
	void pickup_logic( Player* player );

	bool init( int argc, char* argv[] );

	void render();
	bool logic();

	int cleanup();

	void usage( ostream& out = cout );

public:
	int exec( int argc, char* argv[] );
};

#endif
