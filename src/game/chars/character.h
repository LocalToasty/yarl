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

#include <array>
#include <memory>
#include "entity.h"

class Attack;
class Weapon;
class Armor;
class Tile;
class Sector;

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

  Character(Entity&& ent, double speed, int visionRange,
            const Attributes& attributes, Attack* unarmed, int bab = 0);

  bool los(Position pos) const;
  bool los(Entity const& e) const;

  bool move(Position diff);

  virtual void attack(std::shared_ptr<Entity> target);

  int armorClass() const;

  Attack* unarmed();

  Armor* armor();
  Armor const* armor() const;
  void setArmor(Armor* armor);

  int attribute(Attribute attribute) const;
  void setAttribute(Attribute attribute, int value);
  virtual int attributeMod(Attribute attribute) const;

  std::vector<std::shared_ptr<Entity const> > seenEntities() const;

  int visionRange() const;
  double speed() const;

  double lightLoad() const;
  double mediumLoad() const;
  double heavyLoad() const;

  Load load() const;

  int loadMaxDexBon() const;
  int loadCheckPenalty() const;

  std::weak_ptr<Entity> lastTarget() const;
  void setLastTarget(std::weak_ptr<Entity> lastTarget);

  int bab();

 private:
  int _visionRange;

  Attack* _unarmed;
  Armor* _armor{};

  double _speed;

  int _bab;  // base attack bonus

  std::weak_ptr<Entity> _lastTarget{};

 protected:
  Attributes _attributes;
};

#endif
