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

#include <boost/optional.hpp>
#include <memory>
#include "npc.h"

/**
 * @brief An NPC who helps another character.
 *
 * Companion NPCs try to assist another character as much as possible.
 * They follow them around and attack any enemies which attack their friend,
 * or which are attacked by them.
 */
class Companion : public NPC {
 private:
  std::weak_ptr<Character const> _companion;

  boost::optional<Position> _waypoint;

 public:
  Companion(const Tile& t, std::weak_ptr<Character const> companion, int hp,
            Position pos, double speed, int visionRange,
            const Attributes& attributes, Attack* unarmed,
            const std::vector<Item*>& inventory = {}, int bab = 0,
            Size s = Size::medium, int naturalArmor = 0);

  void think();
};

#endif
