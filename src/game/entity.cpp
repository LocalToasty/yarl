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
#include "deathevent.h"
#include "dropevent.h"
#include "item.h"
#include "sector.h"
#include "world.h"

Entity::Entity(Tile const& t, int hp, Position pos, Size s, int naturalArmor,
               std::vector<Item*> const& inventory)
    : _t(t),
      _pos(pos),
      _hp(hp),
      _maxHp(hp),
      _naturalArmor(naturalArmor),
      _s(s),
      _inventory(inventory) {}

std::weak_ptr<Character> Entity::lastAttacker() const { return _lastAttacker; }

int Entity::hp() const { return _hp; }

Entity::Size Entity::size() const { return _s; }

int Entity::naturalArmor() const { return _naturalArmor; }

void Entity::setLastAttacker(std::weak_ptr<Character> lastAttacker) {
  _lastAttacker = lastAttacker;
}

int Entity::maxHp() const { return _maxHp; }

void Entity::setMaxHp(int maxHp) { _maxHp = maxHp; }

const Tile& Entity::t() const { return _t; }

Position Entity::pos() const { return _pos; }

World* Entity::world() { return _world; }

World const* Entity::world() const { return _world; }

bool Entity::seen() const { return _lastKnownPos == boost::none; }

boost::optional<Position> Entity::lastKnownPos() const { return _lastKnownPos; }

void Entity::setLastKnownPos(boost::optional<Position> pos) {
  _lastKnownPos = pos;
}

std::string Entity::prefix() const { return t().prefix; }

std::string Entity::desc() const { return _t.description; }

int Entity::armorClass() const { return 5 + _s + _naturalArmor; }

std::vector<Item*>& Entity::inventory() { return _inventory; }

const std::vector<Item*>& Entity::inventory() const { return _inventory; }

void Entity::setPos(Position pos) {
  _pos = pos;

  if (_world) {
    _world->updateEntity(this);
  }
}

void Entity::setHp(int hp) {
  if (hp <= 0) {
    // Entity is dead
    world()->addEvent(std::make_unique<DeathEvent>(*this));

    // drop inventory
    for (Item* e : _inventory) {
      e->setPos(pos());
      e->setLastKnownPos(boost::none);
      world()->addEvent(std::make_unique<DropEvent>(*this, *e));
    }

    _world->removeEntity(this);
  }

  _hp = hp;
}

void Entity::doDamage(int dmg) { setHp(_hp - dmg); }
