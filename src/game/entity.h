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

#include "tile.h"
#include "vec.hpp"
#include <list>
#include <boost/optional.hpp>

class Item;
class World;
class Sector;
class Character;

class Entity {
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

 private:
  const Tile& _t;

  Vec<int, 2> _pos;

  int _hp;  // hitpoints
  int _maxHp;

  int _naturalArmor;

  Size _s;

  World& _world;
  Sector* _sector;

  // last known coordinates
  boost::optional<Vec<int, 2>> _lastKnownPos;

  std::list<Item*> _inventory;

  Character* _lastAttacker{nullptr};

 public:
  Entity(const Tile& t, int hp, Vec<int, 2> pos, World& world,
         Size s = Size::medium, int naturalArmor = 0,
         const std::list<Item*>& inventory = {});
  virtual ~Entity();

  const Tile& t() const { return _t; }

  Vec<int, 2> pos() const { return _pos; }
  World& world() const { return _world; }
  Sector* sector() const { return _sector; }

  bool seen() const { return _lastKnownPos == boost::none; }
  boost::optional<Vec<int, 2>> lastKnownPos() const { return _lastKnownPos; }
  void setLastKnownPos(boost::optional<Vec<int, 2>> pos);

  std::string prefix() const { return _t.prefix(); }
  std::string desc() const { return _t.desc(); }

  int maxHp() const;
  Character* lastAttacker() const;

  int hp() const { return _hp; }
  Size size() const { return _s; }
  int naturalArmor() const { return _naturalArmor; }
  virtual int armorClass();

  std::list<Item*>& inventory() { return _inventory; }

  void setPos(Vec<int, 2> pos);
  void setSector(Sector* Sector);

  void setHp(int hp);
  void doDamage(int dmg);

  void setLastAttacker(Character* lastAttacker);
  void setMaxHp(int maxHp);
};
#endif
