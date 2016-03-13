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

#include "character.h"
#include "world.h"
#include "sector.h"
#include "yarlconfig.h"
#include "player.h"
#include <attackevent.h>
#include <cstdlib>
#include <cmath>  // for pow

Character::Character(const Tile& t, int hp, Position pos, double speed,
                     int visionRange,
                     const std::array<int, noOfAttributes>& attributes,
                     World& world, Attack* unarmed,
                     const std::list<Item*>& inventory, int bab,
                     Character::Size s, int naturalArmor)
    : Entity(t, hp, pos, world, s, naturalArmor, inventory),
      _visionRange(visionRange),
      _unarmed(unarmed),
      _speed(speed),
      _bab(bab),
      _attributes(attributes) {}

bool Character::move(Position diff) {
  if (world().passable(pos() + diff)) {
    setPos(pos() + diff);
    return true;
  }

  return false;
}

void Character::attack(Entity* target) {
  _lastTarget = target;
  target->setLastAttacker(this);

  // hit roll
  int toHitMod = _bab + attributeMod(strength) + size();
  int hitRoll = rand() % 20 + 1;

  if ((pos() - target->pos()).norm() <= _unarmed->range() &&
      hitRoll + toHitMod >= target->armorClass()) {
    bool crit = false;
    int damage = _unarmed->damage() + attributeMod(strength);

    if (hitRoll >= _unarmed->critRange()) {  // potential crit
      crit = rand() % 20 + 1 + toHitMod >= target->armorClass();

      if (crit) {  // confirm crit
        for (int i = 1; i < _unarmed->critMultiplier(); i++) {
          damage += _unarmed->damage() + attributeMod(strength);
        }
      }
    }

    world().addEvent(std::make_unique<AttackEvent>(*this, *target, true));

    if (damage <= 0) {  // hits inflict at least 1 hp damage
      damage = 1;
    }

    target->setHp(target->hp() - damage);
    return;
  } else {  // don't do any damage on miss
    world().addEvent(std::make_unique<AttackEvent>(*this, *target, false));
  }
}

bool Character::los(Position pos, double factor) const {
  return world().los(this->pos(), pos, _visionRange * factor);
}

bool Character::los(Entity const& e, double factor) const {
  return los(e.pos(), factor);
}

// returns a vector with the entities currently seen by the character
std::vector<Entity*> Character::seenEntities() {
  std::vector<Entity*> ents;

  for (Entity* e : world().entities(
           pos() - Position({visionRange(), visionRange()}),
           pos() + Position({visionRange() + 1, visionRange() + 1}))) {
    if (los(*e)) {
      ents.push_back(e);
    }
  }

  return ents;
}

int Character::armorClass() {
  return 10 + attributeMod(dexterity) + size() + naturalArmor() +
         (_armor == nullptr ? 0 : _armor->ac());
}

Attack* Character::unarmed() { return _unarmed; }

Armor* Character::armor() const { return _armor; }

void Character::setArmor(Armor* armor) { _armor = armor; }

int Character::attribute(Character::Attribute attribute) {
  return _attributes[attribute];
}

int Character::attributeMod(Character::Attribute attribute) {
  int bonus = (_attributes[attribute] - 10) / 2;

  if (_armor) {
    if ((attribute == dexterity || attribute == strength)) {
      if (_armor) {
        bonus += _armor->checkPenalty();
      }

      bonus += loadCheckPenalty();
    }

    if (attribute == dexterity) {
      if (bonus > _armor->maxDexBon()) {
        bonus = _armor->maxDexBon();
      }

      if (bonus > loadMaxDexBon()) {
        bonus = loadMaxDexBon();
      }
    }
  }

  return bonus;
}

int Character::visionRange() const { return _visionRange; }

double Character::speed() const { return _speed; }

double Character::inventoryWeight() {
  double weight = 0;

  for (Item* i : inventory()) {
    weight += i->weight();
  }

  return weight;
}

double Character::lightLoad() { return heavyLoad() / 3; }

double Character::mediumLoad() { return heavyLoad() * 2 / 3; }

double Character::heavyLoad() {
  if (attribute(strength) <= 10) {
    return 10 * attribute(strength);
  } else {
    return 25 * std::pow(2, attribute(strength) / 5);
  }
}

Character::Load Character::load() {
  double weight = inventoryWeight();

  if (weight > heavyLoad()) {
    return Load::overloaded;
  } else if (weight > mediumLoad()) {
    return Load::heavy;
  } else if (weight > lightLoad()) {
    return Load::medium;
  } else {
    return Load::light;
  }
}

int Character::loadMaxDexBon() {
  switch (load()) {
    default:
    case Load::light:
      return 999;  // no dex restriction

    case Load::medium:
      return 3;

    case Load::heavy:
      return 1;

    case Load::overloaded:
      return 0;
  }
}

int Character::loadCheckPenalty() {
  switch (load()) {
    default:
    case Load::light:
      return 0;

    case Load::medium:
      return -3;

    case Load::heavy:
    case Load::overloaded:
      return -6;
  }
}

Entity* Character::lastTarget() const { return _lastTarget; }

void Character::setLastTarget(Entity* lastTarget) { _lastTarget = lastTarget; }

int Character::bab() { return _bab; }
