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

#include "entity.h"
#include <algorithm>
#include <tuple>
#include <cmath>

using namespace std;

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
bool Sector::passable(int x, int y) {
  // check for terrain passability
  if (!tile(x, y)->passable()) {
    return false;
  }

  // check for entitiy passability
  for (Entity* e : entities(x, y))
    if (!e->t().passable()) {
      return false;
    }

  return true;
}

int Sector::size() { return _size; }

const list<Entity*>& Sector::entities() const { return _entities; }

void Sector::addEntity(Entity* e) {
  // Entities which are passable should be drawn before those wich are not.
  // Entities which are transparent should be drawn before those wich are not.
  // This is to ensure that the impassable / opaque entities are drawn on
  // top.
  auto comp = [](Entity* element, Entity* value) {
    return (element->t().passable() && !value->t().passable()) ||
           (element->t().transparent() && !value->t().transparent());
  };

  auto pos = std::upper_bound(_entities.begin(), _entities.end(), e, comp);

  _entities.insert(pos, e);
}

void Sector::removeEntity(Entity* e) { _entities.remove(e); }

vector<Entity*> Sector::entities(int x, int y) const {
  vector<Entity*> ents;

  for (Entity* e : _entities)
    if (e->x() == x && e->y() == y) {
      ents.push_back(e);
    }

  return ents;
}

bool Sector::explored(int x, int y) {
  return _explored.at(x % _size + (y % _size) * _size);
}

void Sector::setExplored(int x, int y, bool explored) {
  _explored.at(x % _size + (y % _size) * _size) = explored;
}

Tile* Sector::tile(int x, int y) {
  return _tiles.at(x % _size + (y % _size) * _size);
}

void Sector::setTile(int x, int y, Tile* tile) {
  _tiles.at(x % _size + (y % _size) * _size) = tile;
}
