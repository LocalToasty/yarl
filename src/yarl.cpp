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
		{ "name",			{ "Adventurer",	"Name of your character." } }
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

		{ 'q', Command::quit }
	};

	// get config file path

	// config path can be supplied via an environment variable
	const char* cfg = getenv("YARLCONF");
	string configFilePath;
	if (cfg != nullptr)
		configFilePath = cfg;

	// if nothing else is specified, the config file is stored in the home
	// directory.
	if (configFilePath.empty())
	{
#if defined( __unix__ ) || defined ( __unix )
		// UNIXoids (i.e. Linux, MacOS, BSD and so forth)
		configFilePath = getenv("HOME");
		configFilePath.append("/.yarlrc");

#elif defined( _WIN32 ) || defined( _WIN64 )	// Windows
		configFilePath = getenv( "HOMEDRIVE" );
		configFilePath.append( getenv( "HOMEPATH" ) );
		configFilePath.append( "\\.yarlrc" );
#endif
	}

	// use user name as default character name
	_variables["name"] = getenv( "USERNAME" );

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
				 << '.' << VERSION_PATCH << " (" << BUILD_TYPE << ")\n"
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
	if (!configFilePath.empty())
	{
		ifstream config(configFilePath);

		if (config.is_open())
		{
			string var;
			while (getline(config, var))
			{
				istringstream iss(var);

				string name;
				iss >> name;

				string val;
				if (!getline(iss, val))
				{
					cerr << "Error: " << configFilePath
						 << ": expected variable value after \""
						 << name << "\"\n";
					return false;
				}

				_variables[name] = val;
			}
		}
	}

#if USE_SDL == ON
	_iom = new SDLIOManager( _variables["color"].toInt() );
#else
	_iom = new CursesIOManager( _variables["color"].toInt() );
#endif

	// create test world
	_world = new World( 5, 5 );

	return true;
}

void Yarl::render()
{
	Character* player = _world->player();
	// get window proportions
	int width = _iom->width();
	int height = _iom->height();

	int offX = width / 2 - player->x();
	int offY = height / 2 - player->y();

	// render the room
	for ( int row = 0; row < height; row++ )
	{
		_iom->moveCursor( 0, row );
		for ( int col = 0; col < width; col++ )
		{
			Tile* t = _world->tile( col - offX, row - offY );
			if (t != nullptr &&
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
						   player->x() + width / 2, player->y() + height / 2 ) )
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
	if (!_world->statusBar().empty())
	{
		_iom->moveAddString( 0, 0,
							 _world->statusBar().getLine(width).c_str() );
		_moreMessages = !_world->statusBar().empty();
	}

	_iom->refreshScreen();
}

bool Yarl::loop()
{
	Character* player = _world->player();

	char input = _iom->getChar();
	Command cmd = _bindings[input];

	if ( cmd == Command::quit || input == 0 || player->hp() <= 0 )
		return true;

	else if ( _moreMessages )
		return false;

	else if ( cmd > Command::MOVEMENT_BEGIN && cmd < Command::MOVEMENT_END )
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

		// TODO
//		_world->letTimePass( ( abs( dx ) + abs( dy ) == 1 ) ? 1 : 1.5 );

		if( !player->move( dx, dy ) )	// an entity is blocking
		{
			auto ents = _world->entities( player->x() + dx, player->y() + dy );
			if( !ents.empty() )
				player->attack( ents.front() );
		}
	}

	else if( cmd == Command::wait )
	{
		// do nothing
	}

	else	// if no correct command was given, don't let the world think
		return false;

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
		   "\t-c, --config <file name>\n\t\t\tconfiguration file to read from.\n";
}

int Yarl::exec(int argc, char* argv[])
{
	if ( !init( argc, argv ) )
	{
		return 0;
	}

	bool finished = false;

	while (!finished)
	{
		render();
		finished = loop();
	}

	return cleanup();
}
