#include "yarl.h"

#include <curses.h>

Tile Yarl::none		= Tile(' ', COLOR_BLACK, "");
Tile Yarl::ground	= Tile('.', COLOR_WHITE, "", true);
Tile Yarl::wallNS	= Tile('|', COLOR_WHITE, "");
Tile Yarl::wallWE	= Tile('-', COLOR_WHITE, "");
Tile Yarl::corridor	= Tile('#', COLOR_BLUE, "", true);
Tile Yarl::tree		= Tile('T', COLOR_GREEN, "");
Tile Yarl::player	= Tile('@', COLOR_YELLOW, "", true, false);

bool Yarl::init()
{
	// create test world
	_currentSector = new Sector(&none);
	_currentSector->createRoom(10, 5, 15, 7,
							  &ground, &wallWE, &wallNS);
	_currentSector->createRoom(40, 7, 10, 6,
							  &ground, &wallWE, &wallNS);
	_currentSector->hLine(24, 8, 17, &corridor);

	new Entity(tree, 17, 8, _currentSector);

	_player = new Character(player, 12, 8, _currentSector);

	// initialize curses
	initscr();
	cbreak();
	noecho();
	curs_set(0);

	// initialize colors
	if (has_colors())
	{
		start_color();
		for (int i = 1; i < 8; i++)
			init_pair(i, i, COLOR_BLACK);
	}

	return true;
}

void Yarl::render()
{
	// render the room
	for (int row = 0; row < _currentSector->height(); row++)
	{
		move(row, 0);
		for (int col = 0; col < _currentSector->width(); col++)
		{
			if(_player->los(col, row))
			{
				// render tiles the character has a LOS to in bold
				_currentSector->setExplored(col, row);
				Tile* t = _currentSector->at(col, row);

				attrset(COLOR_PAIR(t->color()) | A_BOLD);
				addch(t->repr());
			}
			else if(_currentSector->explored(col, row))
			{
				// tiles the player has seen before are rendered in a dim grey
				Tile* t = _currentSector->at(col, row);

				attrset(COLOR_PAIR(COLOR_BLACK) | A_DIM);
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
		if(_player->los(c))
		{
			move(c->y(), c->x());
			attrset(COLOR_PAIR(c->t().color()) | A_BOLD);
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

Yarl::Yarl(int argc, char* argv[])
{

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
