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

#include "world.h"
#include "sector.h"
#include "character.h"
#include "player.h"
#include "companion.h"
#include "item.h"
#include <cmath>
#include <algorithm>

Tile World::_none = {' ', Color::black, "", "void"};
Tile World::_mud = {'.', Color::red, "", "mud", true, true};
Tile World::_grass = {',', Color::green, "a ", "patch of grass", true, true};
Tile World::_tree = {'T', Color::green, "a ", "tree"};
Tile World::_hero = {'@', Color::yellow, "", "you", true, false};
Tile World::_goblin = {'g', Color::green, "a ", "goblin", true, false};
Tile World::_dog = {'d', Color::red, "a ", "dog", true, false};
Tile World::_dogCorpse = {'%', Color::red, "a ", "dog corpse", true, true};
Tile World::_shortSword = {'(', Color::white, "a ", "short sword", true, true};
Tile World::_claymore = {'(', Color::white, "a ", "claymore", true, true};
Tile World::_leatherArmor = {'[',  Color::yellow, "a ", "leather armor",
                             true, true};
Tile World::_buckler = {'[',  Color::red, "a ", "light wooden shield",
                        true, true};

World::World(int width, int height)
    : _width(width), _height(height), _sectors(width * height) {
  for (Sector*& s : _sectors) {
    s = new Sector(&_grass);

    for (int x = 0; x < Sector::size(); x++) {
      for (int y = 0; y < Sector::size(); y++) {
        if (rand() % 16 == 0) {
          s->setTile({x, y}, &_tree);
        } else if (rand() % 8 == 0) {
          s->setTile({x, y}, &_mud);
        }
      }
    }
  }

  std::array<int, 6> attr = {12, 12, 12, 12, 12, 12};
  _player = new Player(_hero, 9 + rand() % 8, {42, 42}, 1, 12, attr, *this,
                       new Attack([]() { return rand() % 2 + 1; }), {}, 1);

  Weapon* weap =
      new Weapon(_shortSword, {[]() { return rand() % 6 + 1; }, 19, 2}, false,
                 2, *this, 5);
  _player->inventory().push_back(weap);
  _player->setMainHand(weap);
  _player->setOffHand(weap);

  Armor* arm = new Armor(_leatherArmor, 2, 6, 0, false, 15, *this);
  _player->inventory().push_back(arm);
  _player->setArmor(arm);

  new Armor(_buckler, 1, 999, -1, true, 5, *this, {43, 43});
  new Weapon(_claymore, {[]() { return rand() % 10 + 1; }, 19, 2, "smite"},
             true, 8, *this, 5, {42, 43});

  attr = {13, 13, 15, 2, 12, 6};
  new Companion(
      _dog, _player, rand() % 8 + 3, {45, 46}, (double)3 / 4, 12, attr, *this,
      new Attack([]() { return rand() % 4 + 2; }),
      {new Item(_dogCorpse, 40, *this, -1, {-1, 1}, Entity::Size::small)}, 2,
      Entity::Size::small, 1);

  attr = {11, 15, 12, 10, 9, 6};
  new Character(_goblin, 1000, {44, 43}, 1, 1, attr, *this,
                new Attack([]() { return rand() % 2 + 1; }));
  new Companion(_goblin, nullptr, rand() % 10 + 2, {45, 45}, 1, 12, attr, *this,
                new Attack([]() { return rand() % 2 + 1; }), {}, 1,
                Entity::Size::small);
}

// TODO simplify
bool World::los(Vec<int, 2> from, Vec<int, 2> to, double range) {
  if (range > 0 && (from - to).norm() > range) {
    return false;
  }

  Vec<int, 2> const diff({to[0] - from[0], to[1] - from[1]});
  Vec<int, 2> const dir({diff[0] > 0 ? 1 : -1, diff[1] > 0 ? 1 : -1});

  std::vector<Entity*> blocking;

  for (Entity* e :
       entities({std::min(from[0], to[0]), std::min(from[1], to[1])},
                {std::max(from[0], to[0]) + 1, std::max(from[1], to[1]) + 1})) {
    if (!e->t().transparent()) {
      blocking.push_back(e);
    }
  }

  if (abs(diff[0]) > abs(diff[1])) {  // in x direction
    int y = from[1];
    int c = std::abs(diff[1]);

    for (int x = from[0]; x != to[0]; x += dir[0]) {
      if (c > abs(diff[0])) {
        y += dir[1];
        c -= abs(diff[0]);
      }

      // check if LOS is broken
      if (!tile({x, y})->transparent()) {
        return false;
      }

      // check if an entity blocks the Line of sight
      for (Entity* e : blocking) {
        if (e->pos() == Vec<int, 2>({x, y})) {
          return false;
        }
      }

      c += abs(diff[1]);
    }
  } else {  // in y direction
    int x = from[0];
    int c = abs(diff[0]);

    for (int y = from[1]; y != to[1]; y += dir[1]) {
      if (c > abs(diff[1])) {
        x += dir[0];
        c -= abs(diff[1]);
      }

      // check if LOS is broken
      if (!tile({x, y})->transparent()) {
        return false;
      }

      for (Entity* e : blocking) {
        if (e->pos() == Vec<int, 2>({x, y})) {
          return false;
        }
      }

      c += abs(diff[0]);
    }
  }
  return true;
}

int min(int x, int y) { return (x < y) ? x : y; }

int max(int x, int y) { return (x > y) ? x : y; }

int heuristic(int x1, int y1, int x2, int y2) {
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);

  return min(dx, dy) + 2 * max(dx, dy);
}

// calculates a route from (x1, y2) to (x2, y2). If converge is true, the path
// will only lead to a tile next to the destination.
std::vector<Command> World::route(Position from, Position dest, bool converge) {
  // A* search (see http://en.wikipedia.org/wiki/A*_search_algorithm)

  struct Node {
    int h;           // heuristic of distance to goal
    int g;           // cost so far
    Command action;  // action taken to reach this node
    Node* parent;    // previous node; nullptr if none
    Position pos;    // coordinates
  };

  // if the destination is not passable, there is no route to it.
  if (!passable(dest) && !converge) {
    return {Command::none};
  }

  // frontier is a priority queue initialised with the starting point
  std::list<Node*> frontier;

  frontier.push_back(new Node{0, 0, Command::none, nullptr, from});

  std::vector<Node*> explored;

  for (;;) {                      // forever
    if (frontier.empty()) {       // failure
      for (Node* n : explored) {  // cleanup
        delete n;
      }

      return {Command::none};
    }

    Node* node = frontier.front();
    frontier.pop_front();

    if (node->pos == dest) {  // optimal path found
      std::vector<Command> directions;

      // get all commands
      for (; node->parent != nullptr; node = node->parent) {
        directions.push_back(node->action);
      }

      // bring them into the right order
      reverse(directions.begin(), directions.end());

      // cleanup
      for (Node* n : frontier) {
        delete n;
      }

      for (Node* n : explored) {
        delete n;
      }

      if (directions.empty()) return {Command::none};

      return directions;
    }

    explored.push_back(node);

    // get all the adjacent nodes
    // I hope this wall of text can somehow be broken down...
    std::vector<std::pair<Command, Position>> passableNeighbours;
    int x = node->pos[0];
    int y = node->pos[1];

    // north
    int ty = y - 1;

    if (passable({x, ty}) || (converge && x == dest[0] && ty == dest[1]))
      passableNeighbours.push_back({Command::north, {x, ty}});

    int tx = x - 1;

    // north west
    if (passable({tx, ty}) || (converge && tx == dest[0] && ty == dest[1]))
      passableNeighbours.push_back({Command::northWest, {tx, ty}});

    // west
    if (passable({tx, y}) || (converge && tx == dest[0] && y == dest[1]))
      passableNeighbours.push_back({Command::west, {tx, y}});

    ty = y + 1;

    // south west
    if (passable({tx, ty}) || (converge && tx == dest[0] && ty == dest[1]))
      passableNeighbours.push_back({Command::southWest, {tx, ty}});

    // south
    if (passable({x, ty}) || (converge && x == dest[0] && ty == dest[1]))
      passableNeighbours.push_back({Command::south, {x, ty}});

    tx = x + 1;

    // south east
    if (passable({tx, ty}) || (converge && tx == dest[0] && ty == dest[1]))
      passableNeighbours.push_back({Command::southEast, {tx, ty}});

    // east
    if (passable({tx, y}) || (converge && tx == dest[0] && y == dest[1]))
      passableNeighbours.push_back({Command::east, {tx, y}});

    ty = y - 1;

    // north east
    if (passable({tx, ty}) || (converge && tx == dest[0] && ty == dest[1]))
      passableNeighbours.push_back({Command::northEast, {tx, ty}});

    for (std::pair<Command, Position> neighbour : passableNeighbours) {
      // if the node is in the explored set, dismiss it
      bool inExplored = false;

      for (Node* n : explored) {
        if (n->pos == neighbour.second) {
          inExplored = true;
          break;
        }
      }

      int movementCost = node->g;

      switch (neighbour.first) {
        case Command::north:
        case Command::south:
        case Command::west:
        case Command::east:
          movementCost = 2;
          break;

        case Command::northWest:
        case Command::northEast:
        case Command::southWest:
        case Command::southEast:
          movementCost = 3;
          break;

        default:
          break;
      }

      int h = heuristic(neighbour.second[0], neighbour.second[1], dest[0],
                        dest[1]) +
              movementCost;

      auto comp = [](const Node* l, const Node* r) { return l->h < r->h; };

      bool inFrontier = false;

      for (Node* n : frontier) {
        if (n->pos == neighbour.second) {
          inFrontier = true;

          // node was already found earlier, but the newly found path
          // to node is shorter
          if (n->g > movementCost) {
            // replace the old node
            delete n;
            frontier.remove(n);

            n = new Node{h, movementCost, neighbour.first, node,
                         neighbour.second};

            auto pos = lower_bound(frontier.begin(), frontier.end(), n, comp);

            frontier.insert(pos, n);
            break;
          }
        }
      }

      // if the node is not yet in the frontier and nor has been found
      // yet, add it to the frontier
      if (!inExplored && !inFrontier) {
        Node* n =
            new Node{h, movementCost, neighbour.first, node, neighbour.second};

        auto pos = lower_bound(frontier.begin(), frontier.end(), n, comp);

        frontier.insert(pos, n);
      }
    }
  }
}

Sector* World::sector(Vec<int, 2> pos) const {
  if (pos[0] >= 0 && pos[1] >= 0 && pos[0] < _width * Sector::size() &&
      pos[1] < _height * Sector::size()) {
    return _sectors.at(pos[0] / Sector::size() +
                       pos[1] / Sector::size() * _width);
  } else {
    return nullptr;
  }
}

Player* World::player() const { return _player; }

Tile* World::tile(Vec<int, 2> pos) const {
  Sector* s = sector(pos);

  if (s) {
    return s->tile(pos);
  } else {
    return nullptr;
  }
}

void World::setTile(Vec<int, 2> pos, Tile* t) {
  Sector* s = sector(pos);

  if (s != nullptr) {
    return s->setTile(pos, t);
  }
}

bool World::explored(Vec<int, 2> pos) const {
  Sector* s = sector(pos);

  if (s) {
    return s->explored(pos);
  } else {
    return false;
  }
}

void World::setExplored(Vec<int, 2> pos, bool explored) {
  Sector* s = sector(pos);

  if (s != nullptr) {
    s->setExplored(pos, explored);
  }
}

bool World::passable(Vec<int, 2> pos) {
  Sector* s = sector(pos);

  if (s != nullptr) {
    return s->passable(pos);
  } else {
    return false;
  }
}

std::vector<Entity*> World::entities(Vec<int, 2> pos) const {
  Sector* s = sector(pos);

  if (s != nullptr) {
    return s->entities(pos);
  } else {
    return std::vector<Entity*>();
  }
}

std::vector<Entity*> World::entities(Vec<int, 2> topLeft,
                                     Vec<int, 2> botRight) {
  std::vector<Entity*> ents;

  for (int x = topLeft[0]; x < botRight[0] + Sector::size();
       x += Sector::size()) {
    for (int y = topLeft[1]; y < botRight[1] + Sector::size();
         y += Sector::size()) {
      Sector* s = sector(Vec<int, 2>({x, y}));

      if (s)
        for (Entity* e : s->entities())
          if (e->pos()[0] >= topLeft[0] && e->pos()[0] < botRight[0] &&
              e->pos()[1] >= topLeft[1] && e->pos()[1] < botRight[1]) {
            ents.push_back(e);
          }
    }
  }

  return ents;
}

void World::addEntitiy(Entity* e) {
  Sector* s = sector(e->pos());

  if (s != nullptr) {
    s->addEntity(e);
  }
}

void World::removeEntity(Entity* e) {
  if (e->sector() != nullptr) {
    e->sector()->removeEntity(e);
  }
}

double World::time() { return _time; }

void World::letTimePass(double time) { _time += time; }

void World::think() {
  for (Entity* e : entities(
           _player->pos() - Vec<int, 2>({Sector::size(), Sector::size()}),
           _player->pos() + Vec<int, 2>({Sector::size(), Sector::size()}))) {
    if (NPC* n = dynamic_cast<NPC*>(e)) {
      double lastAction;

      do {
        lastAction = n->lastAction();

        if (n->hp() > 0 && lastAction < _time) {
          n->think();
        }
      } while (lastAction != n->lastAction());
    }
  }
}

bool World::eventAvailable() const { return !_events.empty(); }

void World::addEvent(std::unique_ptr<Event>&& event) {
  _events.emplace(std::move(event));
}

std::unique_ptr<Event> World::getEvent() {
  std::unique_ptr<Event> res = std::move(_events.front());
  _events.pop();
  return res;
}
