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

#ifndef COMPANION_H
#define COMPANION_H

#include "npc.h"

class Companion : public NPC {
 private:
  Character* _companion;

  int _waypointX{-1};
  int _waypointY{-1};

 public:
  Companion(const Tile& t, Character* companion, int hp, int x, int y,
            double speed, int visionRange,
            const array<int, noOfAttributes>& attributes, World& world,
            Attack* unarmed, const list<Item*>& inventory = {}, int bab = 0,
            Size s = Size::medium, int naturalArmor = 0);

  void think();
};

#endif
