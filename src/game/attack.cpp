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

#include "attack.h"

Attack::Attack(int (*damage)(), int critRange, int critMultiplier,
               string critVerb, double range)
    : _damage(damage),
      _range(range),
      _critRange(critRange),
      _critMultiplier(critMultiplier),
      _critVerb(critVerb) {}

double Attack::range() const { return _range; }

int Attack::damage() const { return _damage(); }

int Attack::critRange() { return _critRange; }

int Attack::critMultiplier() { return _critMultiplier; }

string Attack::critVerb() { return _critVerb; }
