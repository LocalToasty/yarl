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

#include "entity.h"
#include "world.h"
#include "sector.h"
#include "item.h"
#include "deathevent.h"
#include "dropevent.h"

Character* Entity::lastAttacker() const { return _lastAttacker; }

void Entity::setLastAttacker(Character* lastAttacker) {
  _lastAttacker = lastAttacker;
}

int Entity::maxHp() const { return _maxHp; }

void Entity::setMaxHp(int maxHp) { _maxHp = maxHp; }
Entity::Entity(const Tile& t, int hp, Vec<int, 2> pos, World& world, Size s,
               int naturalArmor, const std::list<Item*>& inventory)
    : _t(t),
      _pos(pos),
      _hp(hp),
      _maxHp(hp),
      _naturalArmor(naturalArmor),
      _s(s),
      _world(world),
      _inventory(inventory) {
  _sector = world.sector(pos);

  if (_sector) {
    _sector->addEntity(this);
  }
}

Entity::~Entity() {
  if (_sector) {
    _sector->removeEntity(this);
  }
}

void Entity::setLastKnownPos(boost::optional<Vec<int, 2>> pos) {
  _lastKnownPos = pos;
}

int Entity::armorClass() { return 5 + _s + _naturalArmor; }

void Entity::setPos(Vec<int, 2> pos) {
  _pos = pos;
  setSector(_world.sector(pos));
}

void Entity::setSector(Sector* sector) {
  if (_sector != nullptr) {
    _sector->removeEntity(this);
  }

  if (sector != nullptr) {
    sector->addEntity(this);
  }

  _sector = sector;
}

void Entity::setHp(int hp) {
  if (hp <= 0) {
    world().addEvent(std::make_unique<DeathEvent>(*this));

    // drop inventory
    for (Item* e : _inventory) {
      e->setPos(pos());
      e->setLastKnownPos(boost::none);
      world().addEvent(std::make_unique<DropEvent>(*this, *e));
    }

    _sector->removeEntity(this);
  }

  _hp = hp;
}

void Entity::doDamage(int dmg) { setHp(_hp - dmg); }
