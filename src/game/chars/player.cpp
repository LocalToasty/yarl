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

#include "player.h"
#include "character.h"
#include "item.h"
#include "world.h"

Player::Player(Tile const& t, int hp, Position pos, double speed,
               int visionRange, Attributes& attributes, World& world,
               Attack* unarmed, std::vector<Item*> const& inventory, int bab,
               Size s, int naturalArmor)
    : Humanoid(t, hp, pos, speed, visionRange, attributes, world, unarmed,
               inventory, bab, s, naturalArmor) {}

std::string Player::itemStatus(Item* i) const {
  std::string description = i->desc();

  if (i == mainHand() && i == offHand()) {
    description += " (in both hands)";
  } else if (i == mainHand()) {
    description += " (in main hand)";
  } else if (i == offHand()) {
    description += " (in off hand)";
  } else if (i == (Item*)armor()) {
    description += " (worn)";
  }

  return description;
}
