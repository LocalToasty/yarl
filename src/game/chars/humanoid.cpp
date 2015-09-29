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

#include "humanoid.h"
#include "armor.h"
#include "world.h"
#include "attackevent.h"

Humanoid::Humanoid(const Tile& t, int hp, int x, int y, double speed,
                   int visionRange,
                   const array<int, noOfAttributes>& attributes, World& world,
                   Attack* unarmed, const list<Item*>& inventory, int bab,
                   Size s, int naturalArmor)
    : Character(t, hp, x, y, speed, visionRange, attributes, world, unarmed,
                inventory, bab, s, naturalArmor) {}

void Humanoid::attack(Entity* target) {
  setLastTarget(target);
  target->setLastAttacker(this);

  Weapon* w1 = dynamic_cast<Weapon*>(_mainHand);
  Weapon* w2 = dynamic_cast<Weapon*>(_offHand);

  int toHitMod = bab() + attributeMod(strength) + size();

  if (w1) {  // weapon in main hand
    // if character is fighting with two weapons apply a penalty
    if (_twoWeaponFighting && w2 && w2 != w1) {
      toHitMod -= 6;
    }

    int hitRoll = rand() % 20 + 1;

    // natural 20 is a hit, natural 1 a miss
    if (hitRoll == 20 ||
        (hitRoll != 1 && hitRoll + toHitMod >= target->armorClass())) {
      int damageMod = attributeMod(strength);

      if (w1 == w2 && attributeMod(strength) > 0) {  // wielded in both hands
        damageMod += attributeMod(strength) / 2;
      }

      int damage = w1->damage() + damageMod;

      bool crit = false;

      if (hitRoll >= w1->critRange()) {  // check if there is a potential crit
        int res = rand() % 20 + 1 + toHitMod;
        crit = res >= target->armorClass();

        if (crit) {  // confirm critical hit
          for (int i = 1; i < w1->critMultiplier(); i++) {
            damage += w1->damage() + damageMod;
          }
        }
      }

      world().addEvent(std::make_unique<AttackEvent>(*this, *target, true));
      if (damage <= 0) {
        damage = 1;
      }

      target->doDamage(damage);
    } else {  // miss
      world().addEvent(std::make_unique<AttackEvent>(*this, *target, false));
    }
  }

  if (w2 && w1 != w2 && (!w1 || _twoWeaponFighting)) {
    toHitMod -= 4;  // off hand has a to hit malus
    int hitRoll = rand() % 20 + 1;

    if (hitRoll == 20 ||
        (hitRoll != 1 && hitRoll + toHitMod >= target->armorClass())) {
      // when fighting with an off hand weapon only half the strength bonus is
      // applied
      int damageMod = (attributeMod(strength) > 0)
                          ? attributeMod(strength) / 2
                          : attributeMod(strength);  // strength malus

      int damage = w2->damage() + damageMod;

      bool crit = false;

      if (hitRoll >= w2->critRange()) {  // check if there is a potential crit
        crit = rand() % 20 + 1 + toHitMod >= target->armorClass();

        if (crit) {  // confirm critical hit
          for (int i = 1; i < w2->critMultiplier(); i++) {
            damage += w2->damage() + damageMod;
          }
        }
      }

      world().addEvent(std::make_unique<AttackEvent>(*this, *target, true));

      target->doDamage(damage);
    } else {
      world().addEvent(std::make_unique<AttackEvent>(*this, *target, false));
    }
  }

  else if (!w1 && !w2) {
    Character::attack(target);
  }
}

int Humanoid::armorClass() {
  int ac = Character::armorClass();

  Armor* s = dynamic_cast<Armor*>(_mainHand);

  if (s && s->isShield()) {
    ac += s->ac();
  }

  Armor* s2 = dynamic_cast<Armor*>(_offHand);

  if (s2 && s2->isShield() && s2 != s) {
    ac += s2->ac();
  }

  return ac;
}

int Humanoid::attributeMod(Character::Attribute attribute) {
  int bonus = (_attributes[attribute] - 10) / 2;

  if (attribute == dexterity || attribute == strength) {
    bonus += loadCheckPenalty();

    Armor* s1 = dynamic_cast<Armor*>(_mainHand);
    Armor* s2 = dynamic_cast<Armor*>(_offHand);

    if (attribute == dexterity) {
      if (armor() && bonus > armor()->maxDexBon()) {
        bonus = armor()->maxDexBon();
      }

      if (s1 && s1->isShield() && bonus > s1->maxDexBon()) {
        bonus = s1->maxDexBon();
      }

      if (s2 && s2->isShield() && bonus > s2->maxDexBon()) {
        bonus = s2->maxDexBon();
      }

      if (bonus > loadMaxDexBon()) {
        bonus = loadMaxDexBon();
      }
    }
  }

  return bonus;
}

bool Humanoid::twoWeaponFighting() const { return _twoWeaponFighting; }

void Humanoid::setTwoWeaponFighting(bool twoWeaponFighting) {
  _twoWeaponFighting = twoWeaponFighting;
}

Item* Humanoid::mainHand() const { return _mainHand; }

Item* Humanoid::offHand() const { return _offHand; }

void Humanoid::setMainHand(Item* i) { _mainHand = i; }

void Humanoid::setOffHand(Item* i) { _offHand = i; }

void Humanoid::setBothHands(Item* i) {
  setMainHand(i);
  setOffHand(i);
}
