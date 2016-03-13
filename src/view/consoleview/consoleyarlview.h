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

#ifndef CONSOLEYARLVIEW_H
#define CONSOLEYARLVIEW_H

#include "yarlview.h"
#include "yarlcontroller.h"
#include "world.h"
#include "statusbar.h"
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>

class ConsoleYarlView : public YarlView {
 public:
  ConsoleYarlView(YarlController& controller, World& world);
  virtual ~ConsoleYarlView();

  void run();
  void quit();

  bool yesNoPrompt(std::string query, bool defAnswer);
  int multipleChoiceDialog(std::string const& query,
                           std::vector<std::string> const& possibleAnswers,
                           size_t defAnswer);

  Item* promptItem(std::string const& message, std::list<Item*>::iterator first,
                   std::list<Item*>::iterator last, function<bool(Item*)> pred);

  void showItemList(std::string const& title, std::list<Item*> const& items,
                    std::function<std::string(Item*)> decorator);

  virtual boost::optional<std::pair<int, int>> promptCoordinates();

  void addStatusMessage(std::string const& message);

 protected:
  /*!
   * \brief Retrieves a character from the keyboard.
   *
   * This funtion blocks until the user makes an input if the input buffer is
   * empty.
   *
   * \return a character entered by the user.
   */
  virtual char getChar() = 0;
  virtual void waitForInput() = 0;

  // width of the terminal screen
  virtual int width() const = 0;
  // height of the terminal screen
  virtual int height() const = 0;

  // turn the cursor on or off
  virtual void cursor(bool val) = 0;

  // cursor coordinates
  virtual int cursorX() const = 0;
  virtual int cursorY() const = 0;

  /*!
   * \brief Adds a character to the screen.
   * \param c	character to be added
   * \param col color for the character to be written in
   */
  virtual void addChar(char c, Color col = Color::white) = 0;

  /*!
   * \brief Writes a string to the screen
   * \param str	string to be written
   * \param col	color for the string to be written in
   */
  virtual void addString(string s, Color col = Color::white) = 0;

  /*!
   * \brief Moves the cursor to the specified position
   */
  virtual void moveCursor(int x, int y) = 0;

  // combined functions
  void moveAddChar(int x, int y, char c, Color col = Color::white);
  void moveAddString(int x, int y, string s, Color col = Color::white);

  void handleEvents();

  /*!
   * \brief Clears a part of the screen.
   */
  virtual void clear(int x, int y, int w, int h) = 0;

  // show all changes made to the streambuffer
  virtual void refreshScreen() = 0;

 private:
  void draw();

  void drawStatusBar();
  void drawCharacterInfo();

  bool _running;

  YarlController& _controller;
  World& _world;  // TODO make const

  std::map<char, Command> _bindings;

  StatusBar _statusBar;
};

#endif
