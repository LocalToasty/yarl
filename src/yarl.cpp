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

#include "yarl.h"

#include "yarlconfig.h"
#include "item.h"
#include "character.h"
#include "player.h"
#include "world.h"
#include "npc.h"
#include <stdexcept>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#if USE_SDL == ON
#include "sdliomanager.h"
#else
#include "cursesiomanager.h"
#endif

using namespace std;

bool Yarl::init(int argc, char* argv[])
{
	// initialize variables
	_variables =
	{
		{ "color",			{ "1",	"enable / disable color."} },
		{ "showUnknown",	{ "0",	"draw unexplored areas."} },
		{ "showUnseen",		{ "1",	"draw explored, but currently not seen "
									"areas." } },
		{ "name",			{ "Advent",	"Name of your character." } }
	};

	_bindings =
	{
		{ 'h', Command::west },
		{ 'l', Command::east },
		{ 'k', Command::north },
		{ 'j', Command::south },
		{ 'y', Command::northWest },
		{ 'u', Command::northEast },
		{ 'b', Command::southWest },
		{ 'n', Command::southEast },

		{ '.', Command::wait },
		{ ',', Command::pickup },

		{ 'e', Command::equip },
		{ 't', Command::unequip },

		{ 'd', Command::drop },
		{ 'i', Command::inventory },

		{ '/', Command::examine },

		{ 'q', Command::quit }
	};

	// get config file path

	// config path can be supplied via an environment variable
	const char* cfg = getenv( "YARLCONF" );
	string configFilePath;
	if ( cfg )
		configFilePath = cfg;

	// if nothing else is specified, the config file is stored in the home
	// directory.
	if ( configFilePath.empty() )
	{
#if defined( __unix__ ) || defined ( __unix )
		// UNIXoids (i.e. Linux, MacOS, BSD and so forth)
		if( cfg = getenv( "HOME" ) )
		{
			configFilePath = cfg;
			configFilePath.append( "/.yarlrc" );
		}

#elif defined( _WIN32 ) || defined( _WIN64 )	// Windows
		configFilePath = getenv( "HOMEDRIVE" );
		configFilePath.append( getenv( "HOMEPATH" ) );
		configFilePath.append( "\\.yarlrc" );
#endif
	}

	// use user name as default character name
	if( const char* username = getenv( "USERNAME" ) )
	{
		stringstream ss( username );
		string name;
		ss >> name;	// only use first name
		_variables["name"] = name;
	}

	for( int i = 0; i < argc; i++ )
	{
		string arg = argv[i];

		if( arg == "-h" || arg == "--help" )
		{
			usage();
			return false;
		}

		else if( arg == "-v" || arg == "--version" )
		{
			cout << PROJECT_NAME
				 << " version " << VERSION_MAJOR << '.' << VERSION_MINOR
				 << '.' << VERSION_PATCH;

			if( !string( VERSION_IDENTIFIER ).empty() )
				cout << '-' << VERSION_IDENTIFIER;

			cout << " (" << BUILD_TYPE << ")\n"
				 "Copyright (C) 2015 Marko van Treeck "
				 "<markovantreeck@gmail.com>\n\n"
				 "This program comes with ABSOLUTELY NO WARRANTY. "
				 "It is free software,\nand you are welcome to "
				 "redistribute it under certain conditions.\n"
				 "For further information consult the GNU GPL version 3 or "
				 "later\n<http://gnu.org/licenses/>\n\n";
			return false;
		}

		else if( arg == "-c" || arg == "--config" )
		{
			i++;
			if( i < argc )
				configFilePath == argv[i];
			else
			{
				cerr << "Error: expected configuration file name!\n";

				usage( cerr );
				return false;
			}
		}
	}

	// if there is a potential config file, try to load it
	if ( !configFilePath.empty() )
	{
		ifstream config( configFilePath );

		if( config.is_open() )
		{
			string line;
			while( getline( config, line ) )
			{
				istringstream iss( line );

				string cmd;
				iss >> cmd;

				if( cmd == "set" )
				{
					string name;
					if( !( iss >> name ) )
					{
						cerr << "Error: " << configFilePath
							 << ": expected variable name.\n";
						return false;
					}

					string val;
					if ( !( iss >> val ) )
					{
						cerr << "Error: " << configFilePath
							 << ": expected variable value after \""
							 << name << "\"\n";
						return false;
					}

					_variables[name] = val;
				}
				else if( cmd == "bind" )
				{
					map<string, Command> lut =
					{
						{ "west",	Command::west },
						{ "south",	Command::south },
						{ "north",	Command::north },
						{ "east",	Command::east },
						{ "northEast",	Command::northEast },
						{ "northWest",	Command::northWest },
						{ "southEast",	Command::southEast },
						{ "southWest",	Command::southWest },
						{ "equip",	Command::equip },
						{ "pickup",	Command::pickup },
						{ "drop",	Command::drop },
						{ "inventory",	Command::inventory },
						{ "wait",	Command::wait },
						{ "quit",	Command::quit }
					};

					string keyS;
					if( !( iss >> keyS ) )
					{
						cerr << "Error: " << configFilePath
							 << ": expected key.\n";
						return false;
					}
					char key = keyS.front();

					string command;
					if ( !( iss >> command ) )
					{
						cerr << "Error: " << configFilePath
							 << ": expected command after \""
							 << cmd << "\"\n";
						return false;
					}

					_bindings[key] = lut[command];
				}
				else
				{
					cerr << "Error: " << configFilePath
						 << ": unknown command: \"" << cmd << "\"\n";
				}
			}
		}
	}

#if USE_SDL == ON
	_iom = new SDLIOManager( _variables["color"].toInt() );
#else
	_iom = new CursesIOManager( _variables["color"].toInt() );
#endif

	// seed RNG
	srand( time( 0 ) );

	// create test world
	_world = new World( 5, 5 );

	return true;
}

list<Item*>::iterator getItemWithName( string name, list<Item*>::iterator it,
									   list<Item*>::iterator end )
{
	while( it != end )
	{
		if( ( *it )->desc().substr( 0, name.size() ) == name )
			return it;
		it++;
	}

	return it;
}

void Yarl::render()
{
	Player* player = _world->player();

	// get window proportions
	int width = _iom->width();
	int height = _iom->height();

	int offX = width / 2 - player->x();
	int offY = height / 2 - player->y();

	// render the room
	for ( int row = 0; row < height - 1; row++ )
	{
		_iom->moveCursor( 0, row );
		for ( int col = 0; col < width; col++ )
		{
			Tile* t = _world->tile( col - offX, row - offY );
			if( t != nullptr &&
				player->los( col - offX, row - offY ) )
			{
				// render tiles the character has a LOS to
				_world->setExplored( col - offX, row - offY );

				_iom->addChar( t->repr(), t->color() );
			}
			else if( t != nullptr &&
					 ( _variables["showUnknown"].toInt() ||
					   ( _variables["showUnseen"].toInt() &&
						 _world->explored( col - offX, row - offY ) ) ) )
			{
				// tiles the player has seen before are rendered in grey
				_iom->addChar( t->repr() );
			}
			else
			{
				_iom->addChar(' ');
			}
		}
	}

	// render entities
	for( Entity* e :
		 _world->entities( player->x() - width / 2, player->y() - height / 2,
						   player->x() + width / 2, player->y() + height / 2 ))
	{
		if( player->los( e ))
		{
			e->setSeen();
			e->setLastKnownX();
			e->setLastKnownY();

			_iom->moveAddChar( e->x() + offX, e->y() + offY,
							   e->t().repr(), e->t().color() );
		}
		else if( _variables["showUnknown"].toInt() ||
				 ( _variables["showUnseen"].toInt() &&
				   e->seen() &&
				   !player->los( e->lastKnownX(), e->lastKnownY() ) ) )
		{
			_iom->moveAddChar( e->lastKnownX() + offX, e->lastKnownY() + offY,
							   e->t().repr());
		}
	}

	// render status bar
	if( !_world->statusBar().empty() )
	{
		_iom->moveAddString( 0, 0,
							 _world->statusBar().getLine(width).c_str() );
		if( !_world->statusBar().empty() )
			_state = State::moreMessages;
	}

	// drop prompt
	else if( _state == State::drop )
	{
		_iom->moveAddString( 0, 0, "What do you want to drop? " );
		_iom->addString( _buf, Color::cyan );

		if( !_buf.empty() )
		{
			auto it = getItemWithName( _buf, player->inventory().begin(),
									   player->inventory().end() );
			// check if there is an item with a similar name in the inventory
			if( it != player->inventory().end() )
			{
				// if there is, suggest it
				_iom->addString( (*it)->desc().
								 substr( _buf.size() ) );
			}

			_iom->addChar( ' ' );
		}
	}

	// equip prompt
	else if( _state == State::equip )
	{
		_iom->moveAddString( 0, 0, "What do you want to equip? " );
		_iom->addString( _buf, Color::cyan );

		if( !_buf.empty() )
		{
			// check if there is an item with a similar name in the inventory
			for( auto it = getItemWithName( _buf, player->inventory().begin(),
											player->inventory().end() );
				 it != player->inventory().end();
				 it = getItemWithName( _buf, ++it,
									   player->inventory().end() ) )
			{
				if( dynamic_cast<Armor*>( *it ) ||
					dynamic_cast<Weapon*>( *it ) )
				{
					_iom->addString( (*it)->desc().substr( _buf.size() ) );
					break;
				}
			}

			_iom->addChar( ' ' );
		}
	}

	else if( _state == State::equip_selectHand )
	{
		_iom->moveAddString( 0, 0, "In which hand? [main/off/both] " );
	}

	else if( _state == State::unequip )
	{
		_iom->moveAddString( 0, 0, "What do you want to take off? " );
		_iom->addString( _buf, Color::cyan );

		if( !_buf.empty() )
		{
			// check if there is an item with a similar name in the inventory
			for( auto it = getItemWithName( _buf,
											player->inventory().begin(),
											player->inventory().end() );
				 it != player->inventory().end();
				 it = getItemWithName( _buf, ++it,
									   player->inventory().end() ) )
			{
				if( *it == player->mainHand() ||
					*it == player->offHand() ||
					*it == player->armor() )
				{
					_iom->addString( (*it)->desc().
									 substr( _buf.size() ) );
					break;
				}
			}

			_iom->addChar( ' ' );
		}
	}

	// show inventory
	else if( _state == State::showInventory )
	{
		_iom->moveAddString( 0, 0, "Inventory", Color::yellow );

		vector<Weapon*> weapons;
		vector<Armor*> armor;
		vector<Item*> misc;

		for( Item* i : player->inventory() )
		{
			if( Weapon* w = dynamic_cast<Weapon*>( i ) )
			{
				weapons.push_back( w );
			}
			else if( Armor* a = dynamic_cast<Armor*>( i ) )
			{
				armor.push_back( a );
			}
			else
			{
				misc.push_back( i );
			}
		}

		int row = 1;

		if( !weapons.empty() )
		{
			_iom->moveAddString( 1, row, "Weapons", Color::cyan );
			row++;

			for( Weapon* w : weapons )
			{
				_iom->moveAddString( 2, row, w->t().prefix() + w->desc() );

				if( player->mainHand() == w )
				{
					if( player->offHand() == w )
						_iom->addString( " (in both hands)" );
					else
						_iom->addString( " (in main hand)" );
				}
				else if( player->offHand() == w )
					_iom->addString( " (in off hand)" );

				row++;
			}
		}

		if( !armor.empty() )
		{
			_iom->moveAddString( 1, row, "Armor", Color::cyan );
			row++;

			for( Armor* a : armor )
			{
				_iom->moveAddString( 2, row, a->t().prefix() + a->desc() );

				if( player->armor() == a )
					_iom->addString( " (worn)" );
				else if( player->mainHand() == a )
				{
					if( player->offHand() == a )
						_iom->addString( " (in both hands)" );
					else
						_iom->addString( " (in main hand)" );
				}
				else if( player->offHand() == a )
					_iom->addString( " (in off hand)" );


				row++;
			}
		}

		if( !misc.empty() )
		{
			_iom->moveAddString( 1, row, "Miscellaneous", Color::cyan );
			row++;

			for( Item* i : misc )
			{
				_iom->moveAddString( 2, row, i->t().prefix() + i->desc() );
				row++;
			}
		}
	}

	// character information
	// name
	_iom->moveAddString( 0, height - 1,
						 _variables["name"].toString().substr( 0, 9 ) );

	_iom->clear( 14, height - 1, width - 15, 1 );

	// hp
	_iom->moveAddString( 10, height - 1, "HP: " );

	Color hpCol = Color::green;
	if( player->hp() < player->maxHp() / 4 )
		hpCol = Color::red;
	else if( player->hp() < player->maxHp() / 3 )
		hpCol = Color::yellow;

#ifdef __MINGW32__
	stringstream ss;

	ss << player->hp();
	string buf;
	ss >> buf;
	_iom->addString( buf, hpCol );

	ss.str("");
	ss.clear();
	ss << player->maxHp();
	ss >> buf;
	_iom->addString( "/" + buf );
#else
	_iom->addString( to_string( player->hp() ), hpCol );
	_iom->addString( "/" + to_string( player->maxHp() ) );
#endif

	// ac
#ifdef __MINGW32__
	ss.str("");
	ss.clear();
	ss << player->armorClass();
	ss >> buf;
	_iom->moveAddString( 22, height - 1, "AC: " + buf );
#else
	_iom->moveAddString( 22, height - 1, "AC: " +
						 to_string( player->armorClass() ) );
#endif

	// load
	Character::Load l = player->load();
	if( l == Character::Load::medium )
		_iom->moveAddString( 29, height - 1, "encumbered" );
	else if( l == Character::Load::heavy )
		_iom->moveAddString( 29, height - 1, "burdened" );
	else if( l == Character::Load::overloaded )
		_iom->moveAddString( 29, height - 1, "overloaded" );

	if( _state == State::examine )
		_iom->moveCursor( _x, _y );

	_iom->refreshScreen();
}

// game logic is handled here
bool Yarl::loop()
{
	Player* player = _world->player();

	char input = _iom->getChar();
	Command cmd = _bindings[input];


	if( _state == State::moreMessages )
	{
		_state = State::def;
	}
	else if( _state == State::showInventory )
	{
		_state = State::def;
	}
	else if( _state == State::drop )
	{
		if( input == '\b' )
		{
			if( !_buf.empty() )
				_buf.pop_back();
		}
		else if( input == '\n' )
		{
			auto it = getItemWithName( _buf, player->inventory().begin(),
									   player->inventory().end() );
			if( _buf.empty() )
			{
				_world->statusBar().addMessage( "Never mind." );
			}
			else if( it != player->inventory().end() )
			{
				if( player->armor() == *it )
				{
					// you have to take off armor before you can drop armor
					_world->statusBar().
							addMessage( "You cannot drop worn armor." );
				}
				else
				{
					// unequip item if it's being held
					if( player->mainHand() == *it )
					{
						player->setMainHand( nullptr );
					}
					else if( player->offHand() == *it )
					{
						player->setOffHand( nullptr );
					}

					// drop item
					_world->statusBar().
							addMessage( "You dropped your " +
										(*it)->desc() + '.');

					Character::Load before = player->load();

					player->inventory().remove( *it );
					(*it)->setXY( player->x(), player->y() );

					Character::Load after = player->load();
					if( after != before )
					{
						// TODO: think of better status messages
						if( after == Character::Load::light )
							_world->statusBar().
								addMessage( "Your are now unencumbered." );
						if( after == Character::Load::medium )
							_world->statusBar().
								addMessage( "Your movements are now "
											"encumbered." );
						else if( after == Character::Load::heavy )
							_world->statusBar().
								addMessage( "You are burdened by your load." );
					}

					_world->letTimePass( 1 );
				}
			}
			else
			{
				_world->statusBar().addMessage( "You have no such item." );
			}
			_state = State::def;
		}
		else
		{
			_buf.push_back( input );
		}
	}
	else if( _state == State::equip )
	{
		if( input == '\b' )
		{
			if( !_buf.empty() )
				_buf.pop_back();
		}
		else if( input == '\n' )
		{
			if( _buf.empty() )
			{
				_world->statusBar().addMessage( "Never mind." );
				_state = State::def;
			}
			else
			{
				auto it = getItemWithName( _buf, player->inventory().begin(),
										   player->inventory().end() );
				if( it != player->inventory().end() )
				{
					Armor* a = dynamic_cast<Armor*>( *it );
					if( a && !a->isShield() )
					{
						if( player->armor() )
						{
							_world->statusBar().
								addMessage( "You are already wearing armor." );
						}
						else
						{
							player->setArmor( a );
						}
						_state = State::def;
					}
					else if( a || dynamic_cast<Weapon*>( *it ) )
						_state = State::equip_selectHand;
					else	// item not equippable
					{
						_world->statusBar().
							addMessage( "You have no such item." );
						_state = State::def;
					}
				}
				else
				{
					_world->statusBar().
						addMessage( "You have no such item." );
					_state = State::def;
				}
			}
		}
		else
		{
			_buf.push_back( input );
		}
	}
	else if( _state == State::equip_selectHand )
	{
		auto it = getItemWithName( _buf, player->inventory().begin(),
								   player->inventory().end() );

		if( it != player->inventory().end() )
		{
			if( input == 'm' )	// main hand
			{
				if( player->mainHand() && player->mainHand() != *it &&
					player->mainHand() != player->offHand() )
				{
					_world->statusBar().
						addMessage( "You have to unequip your " +
									player->mainHand()->desc() + " first." );
				}
				else
				{
					player->setMainHand( *it );
					_world->statusBar().
						addMessage( "You now hold the " + (*it)->desc() +
									" in your main hand.");
				}
			}
			else if( input == 'o' )	// off hand
			{
				if( player->offHand() &&
					player->offHand() != player->mainHand() )
				{
					_world->statusBar().
						addMessage( "You have to unequip your " +
									player->offHand()->desc() + " first." );
				}
				else
				{
					player->setOffHand( *it );
					_world->statusBar().
						addMessage( "You now hold the " + (*it)->desc() +
									" in your off hand.");
				}
			}
			else if( input == 'b' )
			{
				if( player->mainHand() || player->offHand() )
				{
					string items;

					if( player->mainHand() )
						items += player->mainHand()->desc();

					if( player->offHand() &&
						player->offHand() != player->mainHand() )
					{
						if( !items.empty() )
							items += " and your ";

						items += player->offHand()->desc();
					}

					_world->statusBar().
						addMessage( "You have to unequip your " + items +
									" first.");
				}
				else
				{
					player->setMainHand( *it );
					player->setOffHand( *it );
					_world->statusBar().
						addMessage( "You now hold the " + (*it)->desc() +
									" in both hands.");
				}
			}
			else
			{
				_world->statusBar().addMessage( "Never mind." );
			}
		}

		_state = State::def;
	}
	else if( _state == State::unequip )
	{
		if( input == '\b' )
		{
			if( !_buf.empty() )
				_buf.pop_back();
		}
		else if( input == '\n' )
		{
			if( _buf.empty() )
			{
				_world->statusBar().addMessage( "Never mind." );
			}
			else
			{
				for( auto it = getItemWithName( _buf,
												player->inventory().begin(),
												player->inventory().end() );
					 it != player->inventory().end();
					 it = getItemWithName( _buf, ++it,
										   player->inventory().end() ) )
				{
					if( player->mainHand() == *it )
						player->setMainHand( nullptr );

					if( player->offHand() == *it )
						player->setOffHand( nullptr );

					if( player->armor() == *it )
						player->setArmor( nullptr );
				}
			}

			_state = State::def;
		}
		else
		{
			_buf.push_back( input );
		}
	}
	else if( _state == State::examine )
	{
		if( cmd > Command::MOVEMENT_BEGIN && cmd < Command::MOVEMENT_END )
		{
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

			_x += dx;
			if( _x > _iom->width() - 1 )
				_x = _iom->width();
			else if( _x < 0 )
				_x = 0;

			_y += dy;
			if( _y > _iom->height() - 2 )
				_y = _iom->height() - 2;
			else if( _y < 0 )
				_y = 0;

		}
		else if( cmd == Command::wait || input == '\n' )	// confirm
		{
			_iom->cursor( false );

			int offX = player->x() - _iom->width() / 2;
			int offY = player->y() - _iom->height() / 2;

			if( _world->explored( _x + offX, _y + offY ) )
			{
				Tile* t = nullptr;

				for( Entity* e : _world->entities( _x + offX, _y + offY ) )
				{
					if( e->seen() )
					{
						t = ( Tile* ) &( _world->
										 entities( _x + offX, _y + offY ).
										 front()->t() );
						break;
					}
				}

				if( !t )	// no entity found
					t = _world->tile( _x + offX, _y + offY );

				_world->statusBar().
					addMessage( string() + t->repr() + " - " + t->prefix() +
								t->desc() );
			}
			else
			{
				_world->statusBar().
					addMessage( "Never mind." );
			}
			_state = State::def;
		}
		else
		{
			_iom->cursor( false );
			_world->statusBar().
				addMessage( "Never mind." );
			_state = State::def;
		}
	}
	else if( cmd == Command::quit || input == 0 || player->hp() <= 0 )
	{
		return true;
	}
	else if( cmd > Command::MOVEMENT_BEGIN && cmd < Command::MOVEMENT_END )
	{
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

		if( !player->move( dx, dy ) )	// an entity is blocking
		{
			auto ents = _world->entities( player->x() + dx, player->y() + dy );
			if( !ents.empty() )
			{
				_world->letTimePass( 2 );
				player->attack( ents.front() );
			}
		}
		else	// movement success
		{
			_world->letTimePass( ( abs( dx ) + abs( dy ) == 1 ) ?
								 player->speed() : 1.5 * player->speed() );
			auto ents = _world->entities( player->x(), player->y() );

			// always at least 1 because player stands here
			if( ents.size() > 1 )
			{
				for( Entity* e : ents )
				{
					if( e != player )
						_world->statusBar().addMessage( "You see a " +
														e->desc() +
														" here.");
				}
			}
		}
	}
	else if( cmd == Command::pickup )
	{
		Character::Load before = player->load();

		for( Entity* e : _world->entities( player->x(), player->y() ) )
		{
			if( dynamic_cast<Item*>( e ) != nullptr )
			{
				_world->removeEntity( e );
				e->setXY( -1, -1 );
				player->inventory().push_back( ( Item* ) e );

				_world->statusBar().addMessage( "You pick up the " +
												e->desc() + '.' );
				_world->letTimePass( 2 );
			}
		}

		Character::Load after = player->load();
		if( after != before )
		{
			// TODO: think of better status messages
			if( after == Character::Load::medium )
				_world->statusBar().
					addMessage( "Your movements are now encumbered." );
			else if( after == Character::Load::heavy )
				_world->statusBar().
					addMessage( "You are burdened by your load." );
			else if( after == Character::Load::overloaded )
				_world->statusBar().
					addMessage( "You are severly overloaded." );
		}
	}
	else if( cmd == Command::equip )
	{
		_buf.clear();
		_state = State::equip;
	}
	else if( cmd == Command::unequip )
	{
		_buf.clear();
		_state = State::unequip;
	}
	else if( cmd == Command::drop )
	{
		_buf.clear();	// clear previous buffer
		_state = State::drop;
	}
	else if( cmd == Command::inventory )
	{
		_state = State::showInventory;
	}
	else if( cmd == Command::examine )
	{
		_x = _iom->width() / 2;
		_y = _iom->height() / 2;
		_iom->cursor( true );
		_state = State::examine;
	}
	else if( cmd == Command::wait )
	{
		// do nothing
		_world->letTimePass( 1 );
	}

	_world->think();

	// continue main loop
	return false;
}

int Yarl::cleanup()
{
	delete _iom;

	return 0;
}

void Yarl::usage( ostream& out )
{
	out << "Usage: " << PROJECT_NAME << " {option}\n\n"
		   "Options:\n"
		   "\t-h, --help\tthis screen.\n"
		   "\t-v, --version\tversion information.\n"
		   "\t-c, --config <file name>\n"
		   "\t\t\tconfiguration file to read from.\n";
}

int Yarl::exec(int argc, char* argv[])
{
	if ( !init( argc, argv ) )
	{
		return 0;
	}

	bool finished = false;

	while ( !finished )
	{
		render();
		finished = loop();
	}

	return cleanup();
}
