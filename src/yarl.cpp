#include "yarl.h"

#include <curses.h>
#include <stdexcept>

using namespace std;

Tile none		= Tile(' ', COLOR_BLACK,	"void");
Tile ground		= Tile('.', COLOR_WHITE,	"a stone floor", true);
Tile wallNS		= Tile('|', COLOR_WHITE,	"a stone wall");
Tile wallWE		= Tile('-', COLOR_WHITE,	"a stone wall");
Tile corridor	= Tile('#', COLOR_BLUE,		"a dark corridor", true);
Tile grass		= Tile(',', COLOR_GREEN,	"a patch of grass", true);
Tile tree		= Tile('T', COLOR_GREEN,	"a tree");
Tile player		= Tile('@', COLOR_YELLOW,	"you", true, false);

Yarl::Yarl(int argc, char *argv[])
{
	// initialize variables
	_variables =
	{
		{"color",		{"1",	"enable / disable color."}},
		{"visibleBold",	{"1",	"draw visible tiles bold."}},
		{"showUnknown",	{"0",	"draw unexplored areas."}},
		{"showUnseen",	{"1",	"draw explored,"
								"but currently not seen areas."}}
	};

	// read variables from commandline
	if (argc % 2 == 0)
		argc --;

	for (int i = 1; i < argc; i += 2)
	{
		try
		{
			_variables[argv[i]] = argv[i + 1];
		}
		catch (logic_error)
		{}
	}

	// create test world
	_currentSector = new Sector(&none);
	_currentSector->createRoom(10, 5, 15, 7,
							   &ground, &wallWE, &wallNS);
	_currentSector->createRoom(40, 7, 10, 6,
							   &ground, &wallWE, &wallNS);
	_currentSector->createRoom(38, 16, 20, 10,
							   &grass, &tree, &tree);

	_currentSector->hLine(24, 8, 17, &corridor);
	_currentSector->vLine(42, 12, 5, &corridor);

	new Entity(tree, 40, 18, _currentSector);
	new Entity(tree, 45, 22, _currentSector);
	new Entity(tree, 51, 19, _currentSector);
	new Entity(tree, 48, 21, _currentSector);
	new Entity(tree, 53, 24, _currentSector);
	new Entity(tree, 47, 32, _currentSector);

	_player = new Character(player, 12, 8, _currentSector);
}

bool Yarl::init()
{
	// initialize curses
	initscr();
	cbreak();
	noecho();
	curs_set(0);

	// initialize colors
	if (has_colors() && _variables["color"].toInt())
	{
		start_color();
		for (int i = 1; i < 8; i++)
			init_pair(i, i, COLOR_BLACK);
	}

	return true;
}

void Yarl::render()
{
	// get window proportions
	int width, height;
	getmaxyx(stdscr, height, width);

	int offX = width / 2 - _player->x();
	int offY = height / 2 - _player->y();

	// render the room
	for (int row = 0; row < height; row++)
	{
		move(row, 0);
		for (int col = 0; col < width; col++)
		{
			Tile* t = _currentSector->at(col - offX, row - offY);
			if (t != nullptr &&
				_player->los(col - offX, row - offY))
			{
				// render tiles the character has a LOS to
				_currentSector->setExplored(col - offX, row - offY);

				attrset(COLOR_PAIR(t->color()) |
						// only draw bold if the variable is set
						(_variables["visibleBold"].toInt() ? A_BOLD : 0));
				addch(t->repr());
			}
			else if(t != nullptr &&
					(_variables["showUnknown"].toInt()	||
					 (_variables["showUnseen"].toInt()	&&
					  _currentSector->explored(col - offX, row - offY))))
			{
				// tiles the player has seen before are rendered in grey
				attrset(COLOR_PAIR(COLOR_WHITE) | A_DIM);
				addch(t->repr());
			}
			else
			{
				addch(' ');
			}
		}
	}

	// render entities
	for (Entity* c : _currentSector->entities())
	{
		int x = c->x() + offX;
		int y = c->y() + offY;

		if (x > 0 && x < width && y > 0 && y < height &&
			_player->los(c))
		{
			move(c->y() + offY, c->x() + offX);
			attrset(COLOR_PAIR(c->t().color()) |
					(_variables["visibleBold"].toInt() ? A_BOLD : 0));
			addch(c->t().repr());
		}
	}

	refresh();
}

bool Yarl::loop()
{
	char input = getch();
	switch (input)
	{
	// movement
	case Command::north:
	case Command::south:
	case Command::west:
	case Command::east:
	case Command::northWest:
	case Command::northEast:
	case Command::southWest:
	case Command::southEast:
		_player->move((Command) input);
		break;

	// quit the application
	case Command::quit:
		return true;
	}

	// continue main loop
	return false;
}

void Yarl::cleanup()
{
	endwin();
}

int Yarl::exec()
{
	if (!init())
	{
		return -1;
	}

	bool finished = false;

	while (!finished)
	{
		render();
		finished = loop();
	}

	cleanup();
}
