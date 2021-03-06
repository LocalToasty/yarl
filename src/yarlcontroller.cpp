﻿/*
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

#include "yarlcontroller.h"
#include "yarlview.h"
#include "yarlviewfactory.h"
#include "yarlconfig.h"
#include "item.h"
#include "character.h"
#include "player.h"
#include "world.h"
#include "npc.h"
#include "dropevent.h"
#include <boost/range/adaptor/reversed.hpp>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

bool YarlController::init(int argc, char* argv[]) {
  // initialize variables
  //	_variables = map<string, Variable> {
  //		{ "color", { "1", "enable / disable color." } },
  //		{ "showUnknown", { "0", "draw unexplored areas." } },
  //		{
  //			"showUnseen", {
  //				"1", "draw explored, but currently not seen "
  //				"areas."
  //			}
  //		},
  //		{ "name", { "Advent", "Name of your character." } }
  //	};

  _bindings = map<char, Command>{{'h', Command::west},
                                 {'l', Command::east},
                                 {'k', Command::north},
                                 {'j', Command::south},
                                 {'y', Command::northWest},
                                 {'u', Command::northEast},
                                 {'b', Command::southWest},
                                 {'n', Command::southEast},

                                 {'.', Command::wait},

                                 {'f', Command::twoWeaponFightingToggle},

                                 {'e', Command::equip},
                                 {'t', Command::unequip},

                                 {',', Command::pickup},
                                 {'d', Command::drop},
                                 {'i', Command::inventory},

                                 {'/', Command::examine},

                                 {'q', Command::quit}};

  // get config file path

  // config path can be supplied via an environment variable
  const char* cfg = getenv("YARLCONF");
  string configFilePath;

  if (cfg) {
    configFilePath = cfg;
  }

  // if nothing else is specified, the config file is stored in the home
  // directory.
  if (configFilePath.empty()) {
#if defined(__unix__) || defined(__unix)

    // UNIXoids (i.e. Linux, MacOS, BSD and so forth)
    if ((cfg = getenv("HOME"))) {
      configFilePath = cfg;
      configFilePath.append("/.yarlrc");
    }

#elif defined(_WIN32) || defined(_WIN64)  // Windows
    configFilePath = getenv("HOMEDRIVE");
    configFilePath.append(getenv("HOMEPATH"));
    configFilePath.append("\\.yarlrc");
#endif
  }

  // use user name as default character name
  if (const char* username = getenv("USERNAME")) {
    stringstream ss(username);
    string name;
    ss >> name;  // only use first name
                 //_variables["name"] = name;
  }

  for (int i = 0; i < argc; i++) {
    string arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      usage();
      return false;
    }

    else if (arg == "-v" || arg == "--version") {
      cout << PROJECT_NAME << " version " << VERSION_MAJOR << '.'
           << VERSION_MINOR << '.' << VERSION_PATCH;

      if (!string(VERSION_IDENTIFIER).empty()) {
        cout << '-' << VERSION_IDENTIFIER;
      }

      if (!string(BUILD_TYPE).empty()) {
        cout << " (" << BUILD_TYPE << ' ';
      }

      cout << "\nCopyright (C) 2015-2016 Marko van Treeck "
              "<markovantreeck@gmail.com>\n\n"
              "This program comes with ABSOLUTELY NO WARRANTY. "
              "It is free software,\nand you are welcome to "
              "redistribute it under certain conditions.\n"
              "For further information consult the GNU GPL version 3 or "
              "later\n<http://gnu.org/licenses/>\n";
      return false;
    }

    else if (arg == "-c" || arg == "--config") {
      i++;

      if (i < argc) {
        configFilePath == argv[i];
      } else {
        cerr << "Error: expected configuration file name!\n";

        usage(cerr);
        return false;
      }
    }
  }

  // if there is a potential config file, try to load it
  if (!configFilePath.empty()) {
    ifstream config(configFilePath);

    if (config.is_open()) {
      string line;

      while (getline(config, line)) {
        istringstream iss(line);

        string cmd;
        iss >> cmd;

        if (cmd == "set") {
          string name;

          if (!(iss >> name)) {
            cerr << "Error: " << configFilePath
                 << ": expected variable name.\n";
            return false;
          }

          string val;

          if (!(iss >> val)) {
            cerr << "Error: " << configFilePath
                 << ": expected variable value after \"" << name << "\"\n";
            return false;
          }

          //_variables[name] = val;
        } else if (cmd == "bind") {
          map<string, Command> lut = {{"west", Command::west},
                                      {"south", Command::south},
                                      {"north", Command::north},
                                      {"east", Command::east},
                                      {"northEast", Command::northEast},
                                      {"northWest", Command::northWest},
                                      {"southEast", Command::southEast},
                                      {"southWest", Command::southWest},
                                      {"equip", Command::equip},
                                      {"pickup", Command::pickup},
                                      {"drop", Command::drop},
                                      {"inventory", Command::inventory},
                                      {"wait", Command::wait},
                                      {"quit", Command::quit}};

          string keyS;

          if (!(iss >> keyS)) {
            cerr << "Error: " << configFilePath << ": expected key.\n";
            return false;
          }

          char key = keyS.front();

          string command;

          if (!(iss >> command)) {
            cerr << "Error: " << configFilePath << ": expected command after \""
                 << cmd << "\"\n";
            return false;
          }

          _bindings[key] = lut[command];
        } else {
          cerr << "Error: " << configFilePath << ": unknown command: \"" << cmd
               << "\"\n";
        }
      }
    }
  }

  // create test world
  _world = make_unique<World>(5, 5);

  _view = makeView(*this, *_world);

  // seed RNG
  srand(time(0));

  return true;
}

int YarlController::cleanup() { return 0; }

void YarlController::usage(ostream& out) {
  out << "Usage: " << PROJECT_NAME
      << " {option}\n\n"
         "Options:\n"
         "\t-h, --help\tthis screen.\n"
         "\t-v, --version\tversion information.\n"
         "\t-c, --config <file name>\n"
         "\t\t\tconfiguration file to read from.\n";
}

int YarlController::exec(int argc, char* argv[]) {
  if (!init(argc, argv)) {
    return 0;
  }

  _view->run();

  return cleanup();
}

void YarlController::quit() {
  bool answer = _view->yesNoPrompt("Do you really want to quit?", false);

  if (answer) {
    _view->quit();
  }
}

void YarlController::moveCommand(Command direction) {
  if (direction == Command::wait) {
    _world->letTimePass(1);
    return;
  }

  Player* player = _world->player();
  int dx = 0;
  int dy = 0;

  if (direction == Command::west || direction == Command::northWest ||
      direction == Command::southWest) {
    dx = -1;
  } else if (direction == Command::east || direction == Command::northEast ||
             direction == Command::southEast) {
    dx = 1;
  }

  if (direction == Command::north || direction == Command::northWest ||
      direction == Command::northEast) {
    dy = -1;
  } else if (direction == Command::south || direction == Command::southWest ||
             direction == Command::southEast) {
    dy = 1;
  }

  if (!player->move(dx, dy)) {  // an entity is blocking
    auto ents = _world->entities(player->x() + dx, player->y() + dy);

    if (!ents.empty()) {
      _world->letTimePass(2);
      player->attack(ents.back());
    }
  } else {  // movement success
    _world->letTimePass((abs(dx) + abs(dy) == 1) ? player->speed()
                                                 : 1.5 * player->speed());
    auto ents = _world->entities(player->x(), player->y());

    // always at least 1 because player stands here
    if (ents.size() > 1) {
      for (Entity* e : boost::adaptors::reverse(ents)) {
        if (e != player) {
          _view->addStatusMessage("You see a " + e->desc() + " here.");
        }
      }
    }
  }
}

void YarlController::showInventory() {
  Player const* const player = _world->player();
  _view->showItemList("Inventory", _world->player()->inventory(),
                      [player](Item* i) { return player->itemStatus(i); });
}

void YarlController::equip() {
  auto player = _world->player();

  // prompt the player for equippable items
  if (boost::optional<Item*> item = _view->promptItem(
          "Select an item to equip:", player->inventory().begin(),
          player->inventory().end(), [](Item* i) {
            return dynamic_cast<Armor*>(i) || dynamic_cast<Weapon*>(i);
          })) {
    Armor* armor = dynamic_cast<Armor*>(*item);
    Weapon* weapon = dynamic_cast<Weapon*>(*item);

    if (armor && !armor->isShield()) {  // item is armor
      if (player->armor()) {
        _view->addStatusMessage("You are already wearing armor.");
      } else {
        player->setArmor(armor);
      }
    } else if (armor || weapon) {  // the item is a shield or a weapon
      if ((weapon && !weapon->twoHanded()) || armor) {
        enum { main_hand = 0, off_hand = 1, both_hands = 2 };
        int hand = _view->multipleChoiceDialog(
            "What hand do you want to equip it in?",
            {"main hand", "off hand", "both hands"}, -1);

        switch (hand) {
          case main_hand:
            player->setMainHand(*item);
            break;

          case off_hand:
            player->setOffHand(*item);
            break;

          case both_hands:
            player->setBothHands(*item);
            break;

          default:
            break;
        }
      } else {  // two handed weapon
        // check if any hands are blocked by other items
        string items;

        if (player->mainHand() && player->mainHand() != item) {
          items += player->mainHand()->desc();
        }

        if (player->offHand() && player->offHand() != item &&
            player->mainHand() != player->offHand()) {
          if (!items.empty()) {
            items += " and your ";
          }

          items += player->offHand()->desc();
        }

        if (!items.empty()) {
          _view->addStatusMessage("You have to unequip your " + items +
                                  " first.");
        } else {
          player->setMainHand(*item);
          player->setOffHand(*item);
          _view->addStatusMessage("You are now holding the " + (*item)->desc() +
                                  " in both hands.");
        }
      }
    } else {  // item not equippable
      _view->addStatusMessage("You have no such item.");
    }
  } else {
    _view->addStatusMessage("You have no such item.");
  }
}

void YarlController::unequip() {
  auto player = _world->player();
  if (boost::optional<Item*> item = _view->promptItem(
          "What item do you want do take off?", player->inventory().begin(),
          player->inventory().end(), [player](Item* i) {
            return player->mainHand() == i || player->offHand() == i ||
                   player->armor() == i;
          })) {
    if (player->mainHand() == *item) {
      player->setMainHand(nullptr);
    }

    if (player->offHand() == *item) {
      player->setOffHand(nullptr);
    }

    if (player->armor() == *item) {
      player->setArmor(nullptr);
    }
  }
}

void YarlController::drop() {
  auto player = _world->player();
  auto inventory = player->inventory();
  if (Item* item =
          _view->promptItem("What item do you want to drop?", inventory.begin(),
                            inventory.end(), [](Item*) { return true; })) {
    if (player->armor() == item) {
      // you have to take off armor before you can drop armor
      _view->addStatusMessage("You cannot drop worn armor.");
    } else {
      // unequip item if it's being held
      if (player->mainHand() == item) {
        player->setMainHand(nullptr);
      } else if (player->offHand() == item) {
        player->setOffHand(nullptr);
      }

      // drop item
      _world->addEvent(std::make_unique<DropEvent>(*player, *item));

      Character::Load before = player->load();

      (item)->setXY(player->x(), player->y());
      player->inventory().remove(item);

      Character::Load after = player->load();

      if (after != before) {
        if (after == Character::Load::light) {
          _view->addStatusMessage("Your are now unencumbered.");
        }

        if (after == Character::Load::medium) {
          _view->addStatusMessage("Your movements are now encumbered.");
        } else if (after == Character::Load::heavy) {
          _view->addStatusMessage("You are burdened by your load.");
        }
      }

      _world->letTimePass(1);
    }
  } else {
    _view->addStatusMessage("Never mind.");
  }
}

void YarlController::examine() {
  // user entered coordinates
  if (auto pos = _view->promptCoordinates()) {
    int x, y;
    std::tie(x, y) = *pos;

    if (_world->explored(x, y)) {
      // find the topmost already seen entity
      // FIXME: if an already seen entity has moved, this doesn't work
      auto const ents = _world->entities(x, y);
      auto e =
          std::find_if(ents.cbegin(), ents.cend(), std::mem_fn(&Entity::seen));

      Tile const* t = nullptr;
      if (e != ents.cend()) {
        t = &(*e)->t();
      } else {  // no entity found, show information on the ground
        t = _world->tile(x, y);
      }

      _view->addStatusMessage(string() + t->repr() + " - " + t->prefix() +
                              t->desc());
    } else {
      _view->addStatusMessage("Unknown");
    }
  } else {
    _view->addStatusMessage("Never mind");
  }
}

void YarlController::twoWeaponFightingToggle() {
  bool b = _world->player()->twoWeaponFighting();
  _world->player()->setTwoWeaponFighting(!b);
}

void YarlController::pickup() {
  Player* player = _world->player();
  Character::Load before = player->load();

  // search for entities in reach of the player
  for (Entity* e : _world->entities(player->x(), player->y())) {
    if (dynamic_cast<Item*>(e) != nullptr) {
      _world->removeEntity(e);
      e->setXY(-1, -1);
      player->inventory().push_back((Item*)e);

      _view->addStatusMessage("You pick up the " + e->desc() + '.');
      _world->letTimePass(2);
    }
  }

  // check if character load has changed; display a message if this is a case
  Character::Load after = player->load();

  if (after != before) {
    switch (after) {
      case Character::Load::light:
        break;

      case Character::Load::medium:
        _view->addStatusMessage("Your movements are now encumbered.");
        break;

      case Character::Load::heavy:
        _view->addStatusMessage("You are burdened by your load.");
        break;

      case Character::Load::overloaded:
        _view->addStatusMessage("You are severly overloaded.");
        break;
    }
  }
}
