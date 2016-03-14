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

  using Attributes = std::array<int, noOfAttributes>;

  enum class Load { light, medium, heavy, overloaded };

  Character(const Tile& t, int hp, Position pos, double speed, int visionRange,
            const Attributes& attributes, World& world, Attack* unarmed,
            const std::vector<Item*>& inventory = {}, int bab = 0,
            Size s = Size::medium, int naturalArmor = 0);

  bool los(Position pos) const;
  bool los(const Entity& e) const;

  bool move(Position diff);

  virtual void attack(Entity* target);

  int armorClass() const;

  Attack* unarmed();

  Armor* armor() const;
  void setArmor(Armor* armor);

  int attribute(Attribute attribute) const;
  void setAttribute(Attribute attribute, int value);
  virtual int attributeMod(Attribute attribute) const;

  std::vector<Entity*> seenEntities();

  int visionRange() const;
  double speed() const;

  double lightLoad() const;
  double mediumLoad() const;
  double heavyLoad() const;

  Load load() const;

  int loadMaxDexBon() const;
  int loadCheckPenalty() const;

  Entity* lastTarget() const;
  void setLastTarget(Entity* lastTarget);

  int bab();

 private:
  int _visionRange;

  Attack* _unarmed;
  Armor* _armor{nullptr};

  double _speed;

  int _bab;  // base attack bonus

  Entity* _lastTarget{nullptr};

 protected:
  Attributes _attributes;
};

#endif
