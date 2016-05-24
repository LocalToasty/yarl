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

std::size_t const Sector::size = 0x20;

Sector::Sector(Tile const* defTile)
    : _tiles(size * size, defTile), _explored(size * size) {}

std::shared_ptr<Entity> Sector::entity(Entity const* ent) {
  auto it = std::find_if(_entities.begin(), _entities.end(),
                         [ent](auto eptr) { return ent == eptr.get(); });
  if (it != _entities.end()) {
    return *it;
  } else {
    return std::shared_ptr<Entity>();
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
  for (auto ent : entities(pos))
    if (!ent->t().passable) {
      return false;
    }

  return true;
}

std::vector<std::shared_ptr<Entity>> Sector::entities() {
  return std::vector<std::shared_ptr<Entity>>{std::begin(_entities),
                                              std::end(_entities)};
}

std::vector<std::shared_ptr<Entity const>> Sector::entities() const {
  return std::vector<std::shared_ptr<Entity const>>{std::begin(_entities),
                                                    std::end(_entities)};
}

std::vector<std::shared_ptr<Entity>> Sector::entities(Position pos) {
  std::vector<std::shared_ptr<Entity>> result;
  std::copy_if(_entities.begin(), _entities.end(), std::back_inserter(result),
               [pos](auto ent) { return ent->pos() == pos; });
  return result;
}

/**
 * @brief Adds an Entity to the sector.
 */
void Sector::addEntity(std::shared_ptr<Entity> ent) {
  // Entities which are passable should be drawn before those wich are not.
  // Entities which are transparent should be drawn before those wich are not.
  // This is to ensure that the impassable / opaque entities are drawn on
  // top.
  auto comp = [](auto element, auto value) {
    return (element->t().passable && !value->t().passable) ||
           (element->t().transparent && !value->t().transparent);
  };

  auto pos = std::upper_bound(_entities.begin(), _entities.end(), ent, comp);

  _entities.insert(pos, ent);
}

void Sector::removeEntity(Entity* ent) {
  std::list<std::shared_ptr<Entity>>::iterator it =
      std::find_if(_entities.begin(), _entities.end(),
                   [ent](auto eptr) { return eptr.get() == ent; });
  _entities.erase(it);
}

std::vector<std::shared_ptr<Entity const>> Sector::entities(
    Position pos) const {
  std::vector<std::shared_ptr<Entity const>> ents;

  for (auto e : _entities)
    if (e->pos() == pos) {
      ents.push_back(e);
    }

  return ents;
}

bool Sector::explored(Position pos) const {
  return _explored.at(pos[0] % size + (pos[1] % size) * size);
}

void Sector::setExplored(Position pos, bool explored) {
  _explored.at(pos[0] % size + (pos[1] % size) * size) = explored;
}

Tile const* Sector::tile(Position pos) const {
  return _tiles.at(pos[0] % size + (pos[1] % size) * size);
}

void Sector::setTile(Position pos, Tile const* tile) {
  _tiles.at(pos[0] % size + (pos[1] % size) * size) = tile;
}
