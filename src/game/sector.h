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
#include <vector>
#include "command.h"
#include "tile.h"
#include "vec.hpp"

class Tile;

// declaration to not run into recursion issiues
class Entity;

class Sector {
 private:
  // size of a sector has to be hardwired so they can be tiled
  static const int _size;

  // vector containing the tiles (stored linearly row for row)
  std::vector<Tile*> _tiles;
  std::vector<bool> _explored;

  // a list of all entities in the sector (i.e. characters, items, props)
  // the bottommost entity has highest render priority
  std::list<Entity*> _entities;

 public:
  Sector(Tile* defTile);
  ~Sector();

  static int size();

  const std::list<Entity*>& entities() const;
  std::vector<Entity*> entities(Position pos) const;
  void addEntity(Entity* e);
  void removeEntity(Entity* e);

  Tile* tile(Position pos);
  void setTile(Position pos, Tile* tile);

  bool passable(Position pos);

  bool explored(Position pos) const;
  void setExplored(Position pos, bool explored = true);
};

#endif
