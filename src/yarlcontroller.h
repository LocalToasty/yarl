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

#ifndef YARL_H
#define YARL_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include "command.h"
#include "sector.h"
#include "world.h"
#include "yarlview.h"

class YarlController {
 private:
  std::unique_ptr<World> _world;
  std::unique_ptr<YarlView> _view;

  std::map<char, Command> _bindings;

  bool init(int argc, char* argv[]);
  void render();
  bool logic();
  int cleanup();
  void usage(std::ostream& out = std::cout);

 public:
  int exec(int argc, char* argv[]);
  void quit();

  void moveCommand(Command direction);
  void equip();
  void unequip();
  void pickup();
  void drop();
  void showInventory();
  void examine();
  void twoWeaponFightingToggle();
};

#endif
