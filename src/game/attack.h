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

#ifndef ATTACK_H
#define ATTACK_H

#include <string>

using namespace std;

class Attack
{
private:
	int (*_damage)();	// damage function
	double _range;

	int _critRange;	// minimum result to score a critical hit
	int _critMultiplier;	// amount by which the damage is multiplied

	string _critVerb;	// word used to describe a critical hit

public:
	Attack(int (*damage)(), int critRange = 20, int critMultiplier = 2,
		string critVerb = "maim", double range = 1.5);

	int damage() const;
	double range() const;
	int critRange();
	int critMultiplier();
	string critVerb();
};

#endif
