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

#include "sector.h"

#include <algorithm>
#include <cmath>
#include <tuple>
#include "entity.h"

const int Sector::_size = 0x20;

Sector::Sector(Tile* defTile)
    : _tiles(_size * _size, defTile), _explored(_size * _size) {}

Sector::~Sector() {
  for (Entity* e : _entities) {
    delete e;
  }
}

// returns true if neither the terrain nor the any entities at the coordinates
// are implassable.
bool Sector::passable(Position pos) const {
  // check for terrain passability
  if (!tile(pos)->passable) {
    return false;
  }

  // check for entitiy passability
  for (Entity* e : entities(pos))
    if (!e->t().passable) {
      return false;
    }

  return true;
}

int Sector::size() { return _size; }

const std::list<Entity*>& Sector::entities() const { return _entities; }

void Sector::addEntity(Entity* e) {
  // Entities which are passable should be drawn before those wich are not.
  // Entities which are transparent should be drawn before those wich are not.
  // This is to ensure that the impassable / opaque entities are drawn on
  // top.
  auto comp = [](Entity* element, Entity* value) {
    return (element->t().passable && !value->t().passable) ||
           (element->t().transparent && !value->t().transparent);
  };

  auto pos = std::upper_bound(_entities.begin(), _entities.end(), e, comp);

  _entities.insert(pos, e);
}

void Sector::removeEntity(Entity* e) { _entities.remove(e); }

std::vector<Entity*> Sector::entities(Position pos) const {
  std::vector<Entity*> ents;

  for (Entity* e : _entities)
    if (e->pos() == pos) {
      ents.push_back(e);
    }

  return ents;
}

bool Sector::explored(Position pos) const {
  return _explored.at(pos[0] % _size + (pos[1] % _size) * _size);
}

void Sector::setExplored(Position pos, bool explored) {
  _explored.at(pos[0] % _size + (pos[1] % _size) * _size) = explored;
}

Tile const* Sector::tile(Position pos) const {
  return _tiles.at(pos[0] % _size + (pos[1] % _size) * _size);
}

void Sector::setTile(Position pos, Tile* tile) {
  _tiles.at(pos[0] % _size + (pos[1] % _size) * _size) = tile;
}
