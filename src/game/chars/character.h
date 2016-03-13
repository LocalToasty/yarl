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

#ifndef CHARACTER_H
#define CHARACTER_H

#include "entity.h"
#include "tile.h"
#include "sector.h"
#include "weapon.h"
#include "armor.h"
#include <array>

class Character : public Entity {
 public:
  enum Attribute {
    strength,
    dexterity,
    constitution,
    intelligence,
    wisdom,
    charisma,
    noOfAttributes
  };

  enum class Load { light, medium, heavy, overloaded };

 private:
  int _visionRange;

  Attack* _unarmed;
  Armor* _armor{nullptr};

  double _speed;

  int _bab;  // base attack bonus

  Entity* _lastTarget{nullptr};

 protected:
  std::array<int, noOfAttributes> _attributes;

 public:
  Character(const Tile& t, int hp, Position pos, double speed, int visionRange,
            const std::array<int, noOfAttributes>& attributes, World& world,
            Attack* unarmed, const std::list<Item*>& inventory = {},
            int bab = 0, Size s = Size::medium, int naturalArmor = 0);

  bool los(Position pos, double factor = 1) const;
  bool los(const Entity& e, double factor = 1) const;
  std::vector<Entity*> seenEntities();

  bool move(Position diff);

  virtual void attack(Entity* target);

  int armorClass();

  Attack* unarmed();

  Armor* armor() const;
  void setArmor(Armor* armor);

  int attribute(Attribute attribute);
  void setAttribute(Attribute attribute, int value);
  virtual int attributeMod(Attribute attribute);

  int visionRange() const;
  double speed() const;

  double inventoryWeight();

  double lightLoad();
  double mediumLoad();
  double heavyLoad();

  Load load();

  int loadMaxDexBon();
  int loadCheckPenalty();

  Entity* lastTarget() const;
  void setLastTarget(Entity* lastTarget);

  int bab();
};

#endif
