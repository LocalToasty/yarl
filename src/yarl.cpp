#include "yarl.h"

#include <curses.h>

const Tile Yarl::none		= Tile(' ', COLOR_WHITE, "");
const Tile Yarl::ground		= Tile('.', COLOR_BLACK, "", true);
const Tile Yarl::wallNS		= Tile('|', COLOR_WHITE, "");
const Tile Yarl::wallWE		= Tile('-', COLOR_WHITE, "");
const Tile Yarl::corridor	= Tile('#', COLOR_BLUE, "", true);
const Tile Yarl::player		= Tile('@', COLOR_RED, "");

bool Yarl::init()
{
	_currentSector = new Sector(80, 20, none);
	_currentSector->createRoom(10, 5, 15, 7,
							  ground, wallWE, wallNS);

	_currentSector->createRoom(40, 7, 10, 6,
							  ground, wallWE, wallNS);

	_currentSector->hLine(24, 8, 17, corridor);

	_player = new Character(player, 12, 8, _currentSector);
	_currentSector->entities().push_back(_player);

	initscr();
	cbreak();
	noecho();
	curs_set(0);

	// initialize colors
	_color = has_colors();
	if (_color)
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
			addch(_currentSector->at(col, row).repr());
		}
	}

	// render entities
	for (Entity* c : _currentSector->entities())
	{
		move(c->y(), c->x());
		if (_color) attron(COLOR_PAIR(c->t().color()));
		addch(c->t().repr());
		if (_color) attroff(COLOR_PAIR(c->t().color()));
	}

	refresh();
}

bool Yarl::loop()
{
	char input = getch();
	switch (input)
	{
	case DIR_N:
	case DIR_S:
	case DIR_W:
	case DIR_E:
		_player->move((Dir) input);
		break;
	case 'q':
		return true;
	}

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
