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

#include "consoleyarlview.h"
#include "yarlcontroller.h"
#include "player.h"
#include "command.h"
#include "attackevent.h"
#include "deathevent.h"
#include "dropevent.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <map>

ConsoleYarlView::ConsoleYarlView(YarlController& controller, World& world)
    : _controller(controller), _world(world) {}

ConsoleYarlView::~ConsoleYarlView() {}

/*!
 * \brief The main io loop.
 *
 * This function contains the main loop of the view.
 * It reads inputs entered by the user and calls the corresponding functons of
 * the model.
 */
void ConsoleYarlView::run() {
  // TODO move to sensible position
  _bindings = {{'h', Command::west},
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

  _running = true;

  while (_running) {
    handleEvents();
    draw();  // render the main game screen

    char const c = getChar();
    auto const cmd = _bindings.find(c);

    // check if c is a valid command
    if (cmd != _bindings.end()) {
      switch (cmd->second) {
        case Command::west:
        case Command::east:
        case Command::north:
        case Command::south:
        case Command::northWest:
        case Command::northEast:
        case Command::southWest:
        case Command::southEast:
        case Command::wait:
          _controller.moveCommand(cmd->second);
          break;

        case Command::twoWeaponFightingToggle:
          _controller.twoWeaponFightingToggle();
          break;

        case Command::equip:
          _controller.equip();
          break;

        case Command::unequip:
          _controller.unequip();
          break;

        case Command::pickup:
          _controller.pickup();
          break;

        case Command::drop:
          _controller.drop();
          break;

        case Command::inventory:
          _controller.showInventory();
          break;

        case Command::examine:
          _controller.examine();
          break;

        case Command::quit:
          _controller.quit();
          break;

        default:
          break;
      }
    } else {
      waitForInput();
    }

    _world.think();
  }
}

/*!
 * \brief Shuts down the ConsoleYarlView object.
 */
void ConsoleYarlView::quit() { _running = false; }

bool ConsoleYarlView::yesNoPrompt(string query, bool defAnswer) {
  return multipleChoiceDialog(query, {"no", "yes"}, defAnswer ? 1 : 0) == 1;
}

int ConsoleYarlView::multipleChoiceDialog(
    const string& query, const std::vector<std::string>& possibleAnswers,
    size_t defAnswer) {
  // show prompt
  moveAddString(0, 0, query);

  std::string buffer;

  while (true) {
    clear(query.size(), 0, width() - cursorX(), 1);

    size_t currChoice;
    if (!buffer.empty()) {  // get the index of the current choice
      for (currChoice = 0; currChoice < possibleAnswers.size(); currChoice++) {
        if (possibleAnswers[currChoice].size() >= buffer.size() &&
            possibleAnswers[currChoice].substr(0, buffer.size()) == buffer) {
          break;
        }
      }
    } else {  // if there is no input, show the default answer
      currChoice = defAnswer;
    }

    moveCursor(query.size() + 1, 0);
    // show user input
    if (!buffer.empty()) {
      addString(buffer, Color::cyan);
    }

    // show hint
    if (currChoice < possibleAnswers.size()) {
      addString(possibleAnswers[currChoice].substr(buffer.size()));
    }

    refreshScreen();

    char input = getChar();

    switch (input) {
      case '\n':
        return currChoice;
        break;

      case '\t':
        break;

      case '\b':
        if (!buffer.empty()) {
          buffer.pop_back();
        }
        break;

      default:
        buffer.push_back(input);
        break;
    }
  }

  return 0;
}

Item* ConsoleYarlView::promptItem(const string& message,
                                  std::list<Item*>::iterator first,
                                  std::list<Item*>::iterator last,
                                  std::function<bool(Item*)> pred) {
  // filter all valid items
  std::vector<Item*> possibleItems;
  std::remove_copy_if(first, last, std::back_inserter(possibleItems),
                      [pred](Item* i) { return !pred(i); });

  // show prompt
  moveAddString(0, 0, message);

  std::string buffer;

  unsigned int index = 0;

  while (true) {
    clear(message.size(), 0, width() - cursorX(), 1);

    auto validItem = [&buffer](Item* i) {
      return i->desc().substr(0, buffer.size()) == buffer;
    };

    // get a matching item
    auto item = std::find_if(first, last, validItem);

    for (size_t i = 0; i < index; ++i) {
      item = std::find_if(++item, last, validItem);
    }

    // show user input
    if (!buffer.empty()) {
      moveAddString(message.size() + 1, 0, buffer, Color::cyan);

      if (item != last) {
        addString((*item)->desc().substr(buffer.size()));
      }
    }

    refreshScreen();

    char input = getChar();

    switch (input) {
      case '\n':
        if (item != last) {  // item found
          return *item;
        } else {  // no item found
          if (buffer.empty()) {
            addStatusMessage("Never mind.");
          }

          return nullptr;
        }
        break;

      case '\t':
        if (std::find_if(++item, last, [&buffer](Item* i) {
              return i->desc().substr(0, buffer.size()) == buffer;
            }) != last) {
          index++;
        } else {
          index = 0;
        }
        break;

      case '\b':
        if (!buffer.empty()) {
          buffer.pop_back();
        }
        break;

      default:
        buffer.push_back(input);
        break;
    }
  }

  return nullptr;
}

void ConsoleYarlView::showItemList(std::string const& title,
                                   std::list<Item*> const& items,
                                   std::function<std::string(Item*)> decorate) {
  clear(0, 0, width(), 1);
  moveAddString(0, 0, title);

  int row = 1;

  for (Item* item : items) {
    moveAddString(2, row, decorate(item));

    row++;
  }

  refreshScreen();
  getChar();
}

boost::optional<std::pair<int, int>> ConsoleYarlView::promptCoordinates() {
  // enable cursor
  int x = width() / 2;
  int y = height() / 2;
  moveCursor(x, y);
  cursor(true);

  // let player move the cursor
  while (true) {
    auto cmd = _bindings.find(getChar());

    if (cmd != _bindings.end()) {
      switch (cmd->second) {
        case Command::wait: {
          // calulate coordinates
          Player const* const player = _world.player();
          cursor(false);
          return std::make_pair(x + player->x() - width() / 2,
                                y + player->y() - height() / 2);
        }

        case Command::cancel:
          cursor(false);
          return boost::none;

        case Command::north:
          y--;
          break;

        case Command::south:
          y++;
          break;

        case Command::west:
          x--;
          break;

        case Command::east:
          x++;
          break;

        case Command::northWest:
          y--;
          x--;
          break;

        case Command::northEast:
          y--;
          x++;
          break;

        case Command::southWest:
          y++;
          x--;
          break;

        case Command::southEast:
          y++;
          x++;
          break;

        default:
          break;
      }
    }

    moveCursor(x, y);
    refreshScreen();
  }
}

void ConsoleYarlView::addStatusMessage(string const& message) {
  _statusBar.addMessage(message);
}

void ConsoleYarlView::moveAddChar(int x, int y, char c, Color col) {
  moveCursor(x, y);
  addChar(c, col);
}

void ConsoleYarlView::moveAddString(int x, int y, string s, Color col) {
  moveCursor(x, y);
  addString(s, col);
}

void ConsoleYarlView::handleEvents() {
  while (_world.eventAvailable()) {
    std::unique_ptr<Event> event = _world.getEvent();
    Player const* const player = _world.player();

    // event is an attack
    if (AttackEvent* attack = dynamic_cast<AttackEvent*>(event.get())) {
      // player attacks
      if (&attack->attacker == player) {
        addStatusMessage(std::string("You ") + (attack->hit ? "hit" : "miss") +
                         " the " + attack->target.desc() + '.');
      }
      // player is attacked
      else if (&attack->target == player) {
        addStatusMessage("The " + attack->attacker.desc() +
                         (attack->hit ? " hits" : " misses") + " you.");
      }
      // player is witnessing an attack
      else if (player->los(attack->attacker) && player->los(attack->target)) {
        addStatusMessage("The " + attack->attacker.desc() +
                         (attack->hit ? " hits" : " misses") + " the " +
                         attack->target.desc() + '.');
      }
    }

    // entity "dies"
    else if (DeathEvent* death = dynamic_cast<DeathEvent*>(event.get())) {
      if (&death->victim == player) {
        addStatusMessage("You die.");
        _running = false;
      } else if (player->los(death->victim)) {
        if (dynamic_cast<Character const*>(&death->victim)) {
          addStatusMessage("The " + death->victim.desc() + " dies.");
        } else {
          addStatusMessage("The " + death->victim.desc() + " is destroyed.");
        }
      }
    }

    // somebody dropped something
    else if (DropEvent* drop = dynamic_cast<DropEvent*>(event.get())) {
      if (drop->dropper.hp() > 0) {
        if (&drop->dropper == player) {
          addStatusMessage("You dropped your " + drop->item.desc() + '.');
        } else if (player->los(drop->dropper)) {
          addStatusMessage("The " + drop->dropper.desc() + " dropped " +
                           drop->item.prefix() + ' ' + drop->item.desc() + '.');
        }
      }
    }
  }
}

/*!
 * \brief Renders the main screen.
 */
void ConsoleYarlView::draw() {
  Player* player = _world.player();

  int offX = width() / 2 - player->x();
  int offY = height() / 2 - player->y();

  // render the map
  for (int row = 0; row < height() - 1; row++) {
    moveCursor(0, row);

    for (int col = 0; col < width(); col++) {
      Tile* t = _world.tile(col - offX, row - offY);

      if (t != nullptr && player->los(col - offX, row - offY)) {
        // render tiles the character has a LOS to
        _world.setExplored(col - offX, row - offY);

        addChar(t->repr(), t->color());
      } else if (t != nullptr && _world.explored(col - offX, row - offY)) {
        // tiles the player has seen before are rendered in grey
        addChar(t->repr());
      } else {
        addChar(' ');
      }
    }
  }

  // render entities
  for (Entity* e :
       _world.entities(player->x() - width() / 2, player->y() - height() / 2,
                       player->x() + width() / 2, player->y() + height() / 2)) {
    if (player->los(*e)) {
      e->setSeen();
      e->setLastKnownX();
      e->setLastKnownY();

      moveAddChar(e->x() + offX, e->y() + offY, e->t().repr(), e->t().color());
    } else if (e->seen() && !player->los(e->lastKnownX(), e->lastKnownY())) {
      moveAddChar(e->lastKnownX() + offX, e->lastKnownY() + offY,
                  e->t().repr());
    }
  }

  drawCharacterInfo();
  refreshScreen();

  drawStatusBar();
}

void ConsoleYarlView::drawStatusBar() {
  while (!_statusBar.empty()) {
    clear(0, 0, width(), 1);
    moveAddString(0, 0, _statusBar.getLine(width()));

    refreshScreen();
    waitForInput();
  }
}

void ConsoleYarlView::drawCharacterInfo() {
  Player* player = _world.player();

  // character information
  // name
  moveAddString(0, height() - 1,
                "foo");  //_variables["name"].toString().substr(0, 9));

  clear(14, height() - 1, width() - 15, 1);

  // hp
  moveAddString(10, height() - 1, "HP: ");

  Color hpCol = Color::green;

  if (player->hp() < player->maxHp() / 4) {
    hpCol = Color::red;
  } else if (player->hp() < player->maxHp() / 2) {
    hpCol = Color::yellow;
  }

  addString(to_string(player->hp()), hpCol);
  addString("/" + to_string(player->maxHp()));

  // ac
  moveAddString(22, height() - 1, "AC: " + to_string(player->armorClass()));

  // load
  Character::Load l = player->load();

  if (l == Character::Load::medium) {
    moveAddString(29, height() - 1, "encumbered");
  } else if (l == Character::Load::heavy) {
    moveAddString(29, height() - 1, "burdened");
  } else if (l == Character::Load::overloaded) {
    moveAddString(29, height() - 1, "overloaded");
  }
}
