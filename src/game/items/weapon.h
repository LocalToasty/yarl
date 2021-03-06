/*
 * YARL - Yet another Roguelike
 * Copyright (C) 2015-2016  Marko van Treeck <markovantreeck@gmail.com>
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

#ifndef WEAPON_H
#define WEAPON_H

#include "item.h"
#include "attack.h"

class Weapon : public Item, public Attack {
 private:
  bool _twoHanded;

 public:
  Weapon(const Tile& t, Attack a, bool twoHanded, double weight, World& world,
         int hp, int x = -1, int y = -1, Size s = Size::small);

  bool twoHanded();
};

#endif
