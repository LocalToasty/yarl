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

#ifndef WORLD_H
#define WORLD_H

#include <map>
#include <memory>
#include <queue>
#include <vector>
#include "command.h"
#include "event.h"
#include "vec.hpp"

class Sector;
class Character;
class Entity;
class Player;
class Weapon;
class Tile;
class Armor;

class World {
 public:
  World(int width, int height);

  static double distance(Position from, Position to);

  bool los(Position from, Position to, double range = -1) const;
  std::vector<Command> route(Position from, Position dest,
                             bool converge = false);

  Sector* sector(Position pos);
  Sector const* sector(Position pos) const;
  std::shared_ptr<Player> player();
  std::shared_ptr<Player const> player() const;

  const Tile* tile(Position pos) const;
  void setTile(Position pos, Tile const* t);

  bool explored(Position pos) const;
  void setExplored(Position pos, bool explored = true);

  bool passable(Position pos) const;

  std::shared_ptr<Entity> entity(Entity* ent);
  std::vector<std::shared_ptr<Entity>> entities(Position pos);
  std::vector<std::shared_ptr<Entity const>> entities(Position pos) const;
  std::vector<std::shared_ptr<Entity>> entities(Position topLeft,
                                                Position botRight);
  std::vector<std::shared_ptr<Entity const>> entities(Position topLeft,
                                                      Position botRight) const;
  void addEntitiy(std::shared_ptr<Entity> ent);
  void removeEntity(Entity* ent);
  void updateEntity(Entity* ent);

  double time();
  void letTimePass(double time);
  void think();

  bool eventAvailable() const;
  void addEvent(std::unique_ptr<Event>&& event);
  std::unique_ptr<Event> getEvent();

 private:
  int _width;
  int _height;

  std::vector<std::unique_ptr<Sector>> _sectors;

  std::shared_ptr<Player> _player;

  double _time{0};

  std::queue<std::unique_ptr<Event>> _events;

  //! Maps each entity to the sector which last owned it.
  std::map<Entity*, Sector*> _entityLocation;

  static Tile _grass;
  static Tile _mud;
  static Tile _tree;
  static Tile _none;

  static Tile _hero;

  static Tile _goblin;

  static Tile _dog;
  static Tile _dogCorpse;

  static Tile _shortSword;
  static Tile _claymore;
  static Tile _leatherArmor;
  static Tile _buckler;
};

#endif
