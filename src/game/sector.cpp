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

/**
 * @brief Retrieves the shared_ptr to an entity.
 *
 * If the entity referred to by the pointer is not in this sector, an
 * empty shared_ptr is returned.
 */
std::shared_ptr<Entity> Sector::entity(Entity const* ent) {
  auto it = std::find_if(_entities.begin(), _entities.end(),
                         [ent](auto eptr) { return ent == eptr.get(); });
  if (it != _entities.end()) {
    return *it;
  } else {
    return std::shared_ptr<Entity>();
  }
}

/**
 * @brief checks if the terrain at the given position is passable.
 *
 * If the position is outside of the sector, undefined behaviour may occur.
 */
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

/**
 * @brief Returns all entities in this sector.
 */
std::vector<std::shared_ptr<Entity>> Sector::entities() {
  return std::vector<std::shared_ptr<Entity>>{std::begin(_entities),
                                              std::end(_entities)};
}

std::vector<std::shared_ptr<Entity const>> Sector::entities() const {
  return std::vector<std::shared_ptr<Entity const>>{std::begin(_entities),
                                                    std::end(_entities)};
}

/**
 * @brief Returns all entities at a certain position.
 * @return
 */
std::vector<std::shared_ptr<Entity>> Sector::entities(Position pos) {
  std::vector<std::shared_ptr<Entity>> result;
  std::copy_if(_entities.begin(), _entities.end(), std::back_inserter(result),
               [pos](auto ent) { return ent->pos() == pos; });
  return result;
}

std::vector<std::shared_ptr<Entity const>> Sector::entities(
    Position pos) const {
  std::vector<std::shared_ptr<Entity const>> result;
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

/**
 * @brief Removes an entity from the sector.
 */
void Sector::removeEntity(Entity* ent) {
  std::list<std::shared_ptr<Entity>>::iterator it =
      std::find_if(_entities.begin(), _entities.end(),
                   [ent](auto eptr) { return eptr.get() == ent; });
  _entities.erase(it);
}

std::size_t Sector::posToIndex(Position pos) const {
  return pos[0] % size + (pos[1] % size) * size;
}

bool Sector::explored(Position pos) const {
  return _explored.at(posToIndex(pos));
}

void Sector::setExplored(Position pos, bool explored) {
  _explored.at(posToIndex(pos)) = explored;
}

Tile const* Sector::tile(Position pos) const {
  return _tiles.at(posToIndex(pos));
}

void Sector::setTile(Position pos, Tile const* tile) {
  _tiles.at(posToIndex(pos)) = tile;
}
