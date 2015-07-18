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

#ifndef HUMANOID_H
#define HUMANOID_H

#include "character.h"

class Item;

class Humanoid : public Character
{
private:
	Item* _mainHand{ nullptr };
	Item* _offHand{ nullptr };

	bool _twoWeaponFighting{ false };

public:
	Humanoid(const Tile& t, int hp, int x, int y, double speed,
			 int visionRange, const array<int, noOfAttributes>& attributes,
			 World& world, Attack* unarmed,
			 const list<Item*>& inventory = {},
			 int bab = 0, Size s = Size::medium, int naturalArmor = 0);

	void attack(Entity* target);
	int armorClass();
	int attributeMod(Attribute attribute);

	Item* mainHand();
	Item* offHand();

	void setMainHand(Item* i);
	void setOffHand(Item* i);
};

#endif
