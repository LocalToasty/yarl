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

#ifndef ENTITY_H
#define ENTITY_H

#include <boost/optional.hpp>
#include <memory>
#include "tile.h"
#include "vec.hpp"

class Item;
class World;
class Sector;
class Character;

class Entity {
  friend class World;

 public:
  enum Size {
    colossal = -8,
    gargantuan = -4,
    huge = -2,
    large = -1,
    medium = 0,
    small = 1,
    tiny = 2,
    diminutive = 4,
    fine = 8
  };

  Entity(Tile const& t, int hp, Position pos, Size s = Size::medium,
         int naturalArmor = 0, std::vector<Item*> const& inventory = {});
  virtual ~Entity() {}

  const Tile& t() const;

  Position pos() const;
  World* world();
  World const* world() const;

  bool seen() const;
  boost::optional<Position> lastKnownPos() const;
  void setLastKnownPos(boost::optional<Position> pos);

  std::string prefix() const;
  std::string desc() const;

  int maxHp() const;
  std::weak_ptr<Character> lastAttacker() const;

  int hp() const;
  Size size() const;
  int naturalArmor() const;
  virtual int armorClass() const;

  std::vector<Item*>& inventory();
  std::vector<Item*> const& inventory() const;

  void setPos(Position pos);

  void setHp(int hp);
  void doDamage(int dmg);

  void setLastAttacker(std::weak_ptr<Character> lastAttacker);
  void setMaxHp(int maxHp);

 private:
  Tile const& _t;

  Position _pos;

  int _hp;  // hitpoints
  int _maxHp;

  int _naturalArmor;

  Size _s;

  World* _world;

  // last known coordinates
  boost::optional<Position> _lastKnownPos;

  std::vector<Item*> _inventory;

  std::weak_ptr<Character> _lastAttacker{};
};

#endif
