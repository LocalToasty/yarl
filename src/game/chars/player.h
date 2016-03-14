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

#ifndef PLAYER_H
#define PLAYER_H

#include "humanoid.h"
#include "weapon.h"
#include <string>

class Player : public Humanoid {
 public:
  Player(Tile const& t, int hp, Position pos, double speed, int visionRange,
         Attributes& attributes, World& world, Attack* unarmed,
         std::vector<Item*> const& inventory = {}, int bab = 0,
         Size s = Size::medium, int naturalArmor = 0);

  std::string itemStatus(Item* i) const;
};

#endif
