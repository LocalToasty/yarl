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

#include "generator.h"
#include "entity.h"
#include "item.h"
#include <cstdlib>	// for rand

Tile Generator::none	= {' ', Color::black,	"void"};
Tile Generator::mud		= {'.', Color::red,		"mud", true};
Tile Generator::grass	= {',', Color::green,	"patch of grass", true};
Tile Generator::tree	= {'T', Color::green,	"tree"};
Tile Generator::bush	= {'o', Color::green,	"bush"};
Tile Generator::log		= {'o', Color::red,		"log"};

Sector* Generator::generateGrassland()
{
	Sector* grassland = new Sector(&grass);

	for (Tile*& t : grassland->_tiles)
	{
		if (rand() % 8 == 0)
		{
			t = &mud;
		}
	}

    int noOfTrees = Sector::size() * Sector::size() / 16;

	for (int i = 0; i < noOfTrees; i++)
	{
        int x = rand() % Sector::size();
        int y = rand() % Sector::size();
		new Entity(tree, x, y, 1, grassland, {new Item(log, 0, 0, 1)});
	}

	return grassland;
}
