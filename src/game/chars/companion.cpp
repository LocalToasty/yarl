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
#include <cstdlib>
#include "attack.h"
#include "world.h"

Companion::Companion(Character&& npc, std::weak_ptr<Character const> companion)
    : NPC(std::move(npc)), _companion(companion) {}

void Companion::think() {
  if (auto attacker = lastAttacker().lock()) {
    // if something attacked, attack back
    setLastTarget(attacker);

    if (lastTarget().lock() == _companion.lock()) {
      // if it was the companion who attacked, the companionship is over
      _companion = std::weak_ptr<Character const>();
    }
  } else if (auto const comp = _companion.lock()) {
    // help companion by attacking either their last target or their last
    // attacker
    if (auto target = comp->lastTarget().lock()) {
      setLastTarget(target);
    } else if (auto target = comp->lastAttacker().lock()) {
      setLastTarget(target);
    }
  }

  // if the current target is not within reach, move towards it
  auto target = lastTarget().lock();
  auto comp = _companion.lock();
  if (target && target->hp() > 0 && los(*target)) {
    _waypoint = target->pos();
  } else if (comp && los(*comp)) {
    _waypoint = *comp->pos() + Position({(rand() % 9) - 4, (rand() % 9) - 4});
  }

  if (_waypoint) {
    // a waypoint is set
    if ((*pos() - *_waypoint).norm() > unarmed()->range()) {
      // target is not in attack range
      auto const route = world().route(*pos(), *_waypoint, true);

      if (!route.empty()) {
        Position const diff(route.front());

        move(diff);

        setLastAction(lastAction() + (abs(diff[0]) + abs(diff[1]) == 1
                                          ? speed()
                                          : 1.5 * speed()));
      }
    } else if (target && target->hp() > 0) {
      // target is in attack range
      attack(target);
      setLastAction(lastAction() + 2);
    }
  } else {
    // do nothing
    setLastAction(world().time());
  }
}
