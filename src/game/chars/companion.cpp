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

#include "companion.h"
#include "world.h"
#include <cstdlib>

Companion::Companion(
    const Tile& t, Character* companion, int hp, Position pos, double speed,
    int visionRange,
    const std::array<int, Character::noOfAttributes>& attributes, World& world,
    Attack* unarmed, const std::vector<Item*>& inventory, int bab,
    Entity::Size s, int naturalArmor)
    : NPC(t, hp, pos, speed, visionRange, attributes, world, unarmed, inventory,
          bab, s, naturalArmor),
      _companion(companion) {}

void Companion::think() {
  if (lastAttacker() != nullptr) {
    setLastTarget(lastAttacker());

    if (lastTarget() == _companion) {
      _companion = nullptr;
    }
  } else if (_companion != nullptr) {
    if (_companion->lastTarget() != nullptr) {
      setLastTarget(_companion->lastTarget());
    } else if (_companion->lastAttacker() != nullptr) {
      setLastTarget(_companion->lastAttacker());
    }
  }

  if (lastTarget() != nullptr && lastTarget()->hp() > 0 && los(*lastTarget())) {
    _waypoint = lastTarget()->pos();
  } else if (_companion != nullptr && los(*_companion)) {
    _waypoint =
        _companion->pos() + Position({(rand() % 9) - 4, (rand() % 9) - 4});
  }

  if (_waypoint[0] >= 0 && _waypoint[0] >= 0) {
    if ((pos() - _waypoint).norm() > unarmed()->range()) {
      auto const route = world().route(pos(), _waypoint, true);

      if (!route.empty()) {
        Position const diff(route.front());

        move(diff);

        setLastAction(lastAction() + (abs(diff[0]) + abs(diff[1]) == 1
                                          ? speed()
                                          : 1.5 * speed()));
      }
    } else if (lastTarget() != nullptr && lastTarget()->hp() > 0) {
      attack(lastTarget());
      setLastAction(lastAction() + 2);
    }
  } else {
    setLastAction(world().time());
  }
}
