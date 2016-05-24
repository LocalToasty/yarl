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

#ifndef ARMOR_H
#define ARMOR_H

#include "item.h"

class Armor : public Item {
 private:
  int _ac;            // armor class bonus
  int _maxDexBon;     // maximum dexterity bonus
  int _checkPenalty;  // penalty imposed onto str / dex checks

  bool _shield;

 public:
  Armor(Tile& t, int ac, int maxDexBon, int checkPenalty, bool shield,
        double weight, Position pos = {-1, -1}, Size s = Size::small);

  int ac() const;
  int maxDexBon() const;
  int checkPenalty() const;
  bool isShield() const;
};

#endif
