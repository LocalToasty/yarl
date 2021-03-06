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

#include "command.h"
#include "tile.h"
#include "weapon.h"
#include "armor.h"
#include "event.h"
#include <vector>
#include <queue>
#include <memory>

class Sector;
class Character;
class Entity;
class Player;

class World {
 public:
  World(int width, int height);

  static double distance(int x1, int y1, int x2, int y2);

  bool los(int x1, int y1, int x2, int y2, double range = -1);
  vector<Command> route(int x1, int y1, int x2, int y2, bool converge = false);

  Sector* sector(int x, int y) const;
  Player* player() const;

  Tile* tile(int x, int y) const;
  void setTile(int x, int y, Tile* t);

  bool explored(int x, int y);
  void setExplored(int x, int y, bool explored = true);

  bool passable(int x, int y);

  vector<Entity*> entities(int x, int y);
  vector<Entity*> entities(int x1, int y1, int x2, int y2);
  void addEntitiy(Entity* e);
  void removeEntity(Entity* e);

  double time();
  void letTimePass(double time);
  void think();

  bool eventAvailable() const;
  void addEvent(std::unique_ptr<Event>&& event);
  std::unique_ptr<Event> getEvent();

 private:
  int _width;
  int _height;

  std::vector<Sector*> _sectors;

  Player* _player;

  double _time{0};

  std::queue<std::unique_ptr<Event>> _events;

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
