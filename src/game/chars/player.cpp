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

Player::Player(Humanoid&& h) : Humanoid(h) {}

std::string Player::itemStatus(Item const* item) const {
  std::string description = item->desc();

  if (item == mainHand() && item == offHand()) {
    description += " (in both hands)";
  } else if (item == mainHand()) {
    description += " (in main hand)";
  } else if (item == offHand()) {
    description += " (in off hand)";
  } else if (item == (Item*)armor()) {
    description += " (worn)";
  }

  return description;
}
