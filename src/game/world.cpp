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

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include "armor.h"
#include "attack.h"
#include "character.h"
#include "companion.h"
#include "item.h"
#include "player.h"
#include "sector.h"
#include "weapon.h"

Tile World::_none = {' ', Color::black, "", "void"};
Tile World::_mud = {'.', Color::red, "", "mud", true, true};
Tile World::_grass = {',', Color::green, "a ", "patch of grass", true, true};
Tile World::_tree = {'T', Color::green, "a ", "tree"};
Tile World::_hero = {'@', Color::yellow, "", "you", true, false};
Tile World::_goblin = {'g', Color::green, "a ", "goblin", true, false};
Tile World::_dog = {'d', Color::red, "a ", "dog", true, false};
Tile World::_dogCorpse = {'%', Color::red, "a ", "dog corpse", true, true};
Tile World::_claymore = {'(', Color::white, "a ", "claymore", true, true};
Tile World::_buckler = {'[',  Color::red, "a ", "light wooden shield",
                        true, true};

std::shared_ptr<Weapon> makeShortSword(World& world) {
  Tile const static shortSword = {'(',           Color::white, "a ",
                                  "short sword", true,         true};
  auto sword = std::make_shared<Weapon>(
      Weapon(Item(Entity(shortSword, 5, world), 2),
             Attack([]() { return rand() % 6 + 1; }, 19, 2), false));
  return sword;
}

std::shared_ptr<Armor> makeLeatherArmor(World& world) {
  Tile const static leatherArmor = {'[',  Color::yellow, "a ", "leather armor",
                                    true, true};
  auto armor = std::make_shared<Armor>(Item(Entity(leatherArmor, 2, world), 15),
                                       2, 6, -2, false);

  return armor;
}

World::World(int width, int height)
    : _width(width), _height(height), _sectors(width * height) {
  for (auto& s : _sectors) {
    s = std::make_unique<Sector>(&_grass);

    for (int x = 0; (std::size_t)x < Sector::size; x++) {
      for (int y = 0; (std::size_t)y < Sector::size; y++) {
        if (rand() % 16 == 0) {
          s->setTile({x, y}, &_tree);
        } else if (rand() % 8 == 0) {
          s->setTile({x, y}, &_mud);
        }
      }
    }
  }

  std::array<int, 6> attr = {12, 12, 12, 12, 12, 12};
  _player = std::make_shared<Player>(Player(Humanoid(
      Character(Entity(_hero, 9 + rand() % 8, *this, Position({42, 42})), 1, 12,
                attr, new Attack([]() { return rand() % 2 + 1; }), 1))));
  addEntity(_player);

  auto sword = makeShortSword(*this);
  sword->setPos(Position({43, 43}));
  addEntity(sword);

  auto leatherArmor = makeLeatherArmor(*this);
  // Armor* arm = new Armor(_leatherArmor, 2, 6, 0, false, 15);
  //_player->inventory().push_back(arm);
  //_player->setArmor(arm);

  // new Armor(_buckler, 1, 999, -1, true, 5, {43, 43});
  // new Weapon(_claymore, {[]() { return rand() % 10 + 1; }, 19, 2, "smite"},
  //           true, 8, 5, {42, 43});

  // attr = {13, 13, 15, 2, 12, 6};
  // new Companion(_dog, _player, rand() % 8 + 3, {45, 46}, (double)3 / 4, 12,
  //              attr, new Attack([]() { return rand() % 4 + 2; }),
  //              {new Item(_dogCorpse, 40, -1, {-1, 1}, Entity::Size::small)},
  //              2,
  //              Entity::Size::small, 1);

  // attr = {11, 15, 12, 10, 9, 6};
  // new Companion(_goblin, nullptr, 1000, {44, 43}, 1, 1, attr,
  //              new Attack([]() { return rand() % 2 + 1; }));
}

// TODO simplify
bool World::los(Position from, Position to, double range) const {
  if (range > 0 && (from - to).norm() > range) {
    return false;
  }

  Vec<int, 2> const diff({to[0] - from[0], to[1] - from[1]});
  Vec<int, 2> const dir({diff[0] > 0 ? 1 : -1, diff[1] > 0 ? 1 : -1});

  std::vector<std::shared_ptr<Entity const>> blocking;

  for (auto ent :
       entities({std::min(from[0], to[0]), std::min(from[1], to[1])},
                {std::max(from[0], to[0]) + 1, std::max(from[1], to[1]) + 1})) {
    if (!ent->t().transparent) {
      blocking.push_back(ent);
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
      if (!tile({x, y})->transparent) {
        return false;
      }

      // check if an entity blocks the Line of sight
      for (auto e : blocking) {
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
      if (!tile({x, y})->transparent) {
        return false;
      }

      for (auto e : blocking) {
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

int heuristic(Position from, Position to) {
  int dx = abs(to[0] - from[0]);
  int dy = abs(to[1] - from[1]);

  return min(dx, dy) + 2 * max(dx, dy);
}

/*! Finds all passable neighbors of the given tile */
std::vector<Position> passableNeighbours(World const& world, Position pos,
                                         Position dest, bool converge) {
  std::vector<Position> res;
  for (int y = -1; y <= 1; y++) {
    for (int x = -1; x <= 1; x++) {
      if (x == 0 && y == 0) {
        continue;
      }

      Position const p = pos + Position({x, y});

      if (world.passable(p) || (converge && p == dest)) {
        res.push_back(p);
      }
    }
  }

  return res;
}

// calculates a route from (x1, y2) to (x2, y2). If converge is true, the path
// will only lead to a tile next to the destination.
std::vector<Command> World::route(Position from, Position dest, bool converge) {
  // A* search (see http://en.wikipedia.org/wiki/A*_search_algorithm)

  struct Node {
    int h;                            // heuristic of distance to goal
    int g;                            // cost so far
    boost::optional<Command> action;  // action taken to reach this node
    std::shared_ptr<Node> parent;     // previous node; nullptr if none
    Position pos;                     // coordinates

    Node(int h, int g, boost::optional<Command> action,
         std::shared_ptr<Node> parent, Position pos)
        : h(h), g(g), action(action), parent(parent), pos(pos) {}
  };

  // if the destination is not passable, there is no route to it.
  if (!passable(dest) && !converge) {
    return {};
  }

  // frontier is a priority queue initialised with the starting point
  std::list<std::shared_ptr<Node>> frontier;

  frontier.emplace_back(
      std::make_shared<Node>(0, 0, boost::none, nullptr, from));

  std::vector<std::shared_ptr<Node>> explored;

  while (true) {
    if (frontier.empty()) {  // failure
      return {};
    }

    std::shared_ptr<Node> node = frontier.front();
    frontier.pop_front();

    if (node->pos == dest) {  // optimal path found
      std::vector<Command> directions;

      // get all commands
      for (; node->parent != nullptr; node = node->parent) {
        if (auto action = node->action) {
          directions.push_back(*action);
        }
      }

      // bring them into the right order
      reverse(directions.begin(), directions.end());

      return directions;
    }

    explored.push_back(node);

    auto successors = passableNeighbours(*this, node->pos, dest, converge);

    for (Position neighbour : successors) {
      // if the node is in the explored set, dismiss it
      bool inExplored = false;

      for (std::shared_ptr<Node> n : explored) {
        if (n->pos == neighbour) {
          inExplored = true;
          break;
        }
      }

      int movementCost = node->g;

      Command const direction = *(neighbour - node->pos).asDirection();
      switch (direction) {
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

      int h = heuristic(neighbour, dest) + movementCost;

      auto comp = [](std::shared_ptr<Node> const l,
                     std::shared_ptr<Node> const r) { return l->h < r->h; };

      bool inFrontier = false;

      for (std::shared_ptr<Node> n : frontier) {
        if (n->pos == neighbour) {
          inFrontier = true;

          // node was already found earlier, but the newly found path
          // to node is shorter
          if (n->g > movementCost) {
            // replace the old node
            n = std::make_shared<Node>(h, movementCost, direction, node,
                                       neighbour);

            auto pos = lower_bound(frontier.begin(), frontier.end(), n, comp);

            frontier.insert(pos, n);
            break;
          }
        }
      }

      // if the node is not yet in the frontier and nor has been found
      // yet, add it to the frontier
      if (!inExplored && !inFrontier) {
        auto n =
            std::make_shared<Node>(h, movementCost, direction, node, neighbour);

        auto pos = lower_bound(frontier.begin(), frontier.end(), n, comp);

        frontier.insert(pos, n);
      }
    }
  }
}

Sector const* World::sector(Position pos) const {
  if (pos[0] >= 0 && pos[1] >= 0 && (size_t)pos[0] < _width * Sector::size &&
      (size_t)pos[1] < _height * Sector::size) {
    // position is within bounds
    return _sectors.at(pos[0] / Sector::size + pos[1] / Sector::size * _width)
        .get();
  } else {
    return nullptr;
  }
}

Sector* World::sector(Position pos) {
  if (pos[0] >= 0 && pos[1] >= 0 && (size_t)pos[0] < _width * Sector::size &&
      (size_t)pos[1] < _height * Sector::size) {
    return _sectors.at(pos[0] / Sector::size + pos[1] / Sector::size * _width)
        .get();
  } else {
    return nullptr;
  }
}

std::shared_ptr<Player const> World::player() const { return _player; }
std::shared_ptr<Player> World::player() { return _player; }

Tile const* World::tile(Position pos) const {
  Sector const* s = sector(pos);

  if (s) {
    return s->tile(pos);
  } else {
    return nullptr;
  }
}

void World::setTile(Position pos, Tile const* t) {
  Sector* s = sector(pos);

  if (s != nullptr) {
    return s->setTile(pos, t);
  }
}

bool World::explored(Position pos) const {
  Sector const* s = sector(pos);

  if (s) {
    return s->explored(pos);
  } else {
    return false;
  }
}

void World::setExplored(Position pos, bool explored) {
  Sector* s = sector(pos);

  if (s != nullptr) {
    s->setExplored(pos, explored);
  }
}

bool World::passable(Position pos) const {
  Sector const* s = sector(pos);

  if (s != nullptr) {
    return s->passable(pos);
  } else {
    return false;
  }
}

/**
 * @brief Gets the shared pointer of an Entity
 */
std::shared_ptr<Entity> World::entity(Entity* ent) {
  auto it = _entityLocation.find(ent);

  if (it != _entityLocation.end()) {
    return it->second->entity(ent);
  } else {
    return std::shared_ptr<Entity>();
  }
}

std::vector<std::shared_ptr<Entity>> World::entities(Position pos) {
  Sector* s = sector(pos);

  if (s != nullptr) {
    return s->entities(pos);
  } else {
    return std::vector<std::shared_ptr<Entity>>();
  }
}

std::vector<std::shared_ptr<Entity const>> World::entities(Position pos) const {
  Sector const* s = sector(pos);

  if (s != nullptr) {
    return s->entities(pos);
  } else {
    return std::vector<std::shared_ptr<Entity const>>();
  }
}

/**
 * @brief Collects all entities positioned within the given boundaries
 * @returns a vector containing the entries
 */
std::vector<std::shared_ptr<Entity>> World::entities(Position topLeft,
                                                     Position botRight) {
  std::vector<std::shared_ptr<Entity>> ents;

  for (int x = topLeft[0]; x < botRight[0] + (int)Sector::size;
       x += Sector::size) {
    for (int y = topLeft[1]; y < botRight[1] + (int)Sector::size;
         y += Sector::size) {
      Sector* s = sector(Vec<int, 2>({(int)x, (int)y}));

      if (s) {
        for (auto ent : s->entities()) {
          if ((*ent->pos())[0] >= topLeft[0] &&
              (*ent->pos())[0] < botRight[0] &&
              (*ent->pos())[1] >= topLeft[1] &&
              (*ent->pos())[1] < botRight[1]) {
            ents.push_back(ent);
          }
        }
      }
    }
  }

  return ents;
}

std::vector<std::shared_ptr<Entity const>> World::entities(
    Position topLeft, Position botRight) const {
  std::vector<std::shared_ptr<Entity const>> ents;

  for (size_t x = topLeft[0]; x < botRight[0] + Sector::size;
       x += Sector::size) {
    for (size_t y = topLeft[1]; y < botRight[1] + Sector::size;
         y += Sector::size) {
      Sector const* s = sector(Position({(int)x, (int)y}));

      if (s) {
        for (auto ent : s->entities()) {
          if ((*ent->pos())[0] >= topLeft[0] &&
              (*ent->pos())[0] < botRight[0] &&
              (*ent->pos())[1] >= topLeft[1] &&
              (*ent->pos())[1] < botRight[1]) {
            ents.push_back(ent);
          }
        }
      }
    }
  }

  return ents;
}

/**
 * @brief Adds an Entity to the world.
 *
 * The Entity mustn't be in the world before being added, else undefined
 * behaviour may occur.
 */
void World::addEntity(std::shared_ptr<Entity> ent) {
  // check if the position is valid
  if (ent->pos() != boost::none) {
    if (Sector* s = sector(*ent->pos())) {
      s->addEntity(ent);
      _entityLocation[ent.get()] = s;
    }
  } else {
    std::cerr << "Tried to add " << ent->desc() << " with no position"
              << std::endl;
    // TODO throw error
  }
}

/**
 * @brief Updates an Entities storage location.
 *
 * Whenever an Entity changes its location, World::updateEntity() must
 * be called.  Not doing so can result in undefined behaviour.
 */
void World::updateEntity(Entity* ent) {
  auto it = _entityLocation.find(ent);

  if (it != _entityLocation.end()) {
    // entity already exists in this world

    // move entity
    Sector* oldSector = it->second;
    Sector* newSector = sector(*ent->pos());
    auto tmp = oldSector->entity(ent);
    oldSector->removeEntity(ent);
    newSector->addEntity(tmp);

    // update entity location table
    it->second = newSector;
  } else {
    // TODO throw error
  }
}

/**
 * @brief Removes an Entity from the world.
 */
void World::removeEntity(Entity* ent) {
  auto it = _entityLocation.find(ent);

  if (it != _entityLocation.end()) {
    // entity is known to the world
    Sector* s = it->second;
    s->removeEntity(ent);
    _entityLocation.erase(ent);
  }
}

double World::time() { return _time; }

void World::letTimePass(double time) { _time += time; }

void World::think() {
  for (auto e :
       entities(*_player->pos() -
                    Vec<int, 2>({(int)Sector::size, (int)Sector::size}),
                *_player->pos() +
                    Vec<int, 2>({(int)Sector::size, (int)Sector::size}))) {
    if (NPC* n = dynamic_cast<NPC*>(e.get())) {
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
