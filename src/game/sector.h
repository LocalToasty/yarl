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

#ifndef SECTOR_H
#define SECTOR_H

#include <list>
#include <memory>
#include <vector>
#include "vec.hpp"

class Tile;
class Entity;

class Sector {
 public:
  Sector(Tile const* defTile);

  std::shared_ptr<Entity> entity(Entity const* ent);
  std::shared_ptr<Entity const> entity(Entity const* ent) const;
  std::vector<std::shared_ptr<Entity>> entities();
  std::vector<std::shared_ptr<Entity const>> entities() const;
  std::vector<std::shared_ptr<Entity>> entities(Position pos);
  std::vector<std::shared_ptr<Entity const>> entities(Position pos) const;
  void addEntity(std::shared_ptr<Entity> ent);
  void removeEntity(Entity* e);

  Tile const* tile(Position pos) const;
  void setTile(Position pos, Tile const* tile);

  bool passable(Position pos) const;

  bool explored(Position pos) const;
  void setExplored(Position pos, bool explored = true);

  // size of a sector has to be hardwired so they can be tiled
  std::size_t const static size;

 private:
  // vector containing the tiles (stored linearly row for row)
  std::vector<Tile const*> _tiles;
  std::vector<bool> _explored;

  // a list of all entities in the sector (i.e. characters, items, props)
  // the bottommost entity has highest render priority
  std::list<std::shared_ptr<Entity>> _entities;
};

#endif
