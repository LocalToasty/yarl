/*
 * YARL - Yet another Roguelike
 * Copyright (C) 2015  Marko van Treeck <markovantreeck@gmail.com>
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

#ifndef CURSESYARLVIEW_H
#define CURSESYARLVIEW_H

#include "consoleyarlview.h"

class CursesYarlView : public ConsoleYarlView {
 public:
  CursesYarlView(YarlController& controller, World& world);
  ~CursesYarlView();

 protected:
  char getChar();
  void waitForInput();

  // width of the terminal screen
  int width() const;
  // height of the terminal screen
  int height() const;

  // turn the cursor on or off
  void cursor(bool val);

  // cursor coordinates
  int cursorX() const;
  int cursorY() const;

  // replace the character at the current cursor location
  void addChar(char c, Color col = Color::white);
  // write a string of characters
  void addString(string s, Color col = Color::white);

  // move the cursor to the given coordinates
  void moveCursor(int x, int y);

  void clear(int x, int y, int w, int h);

  // show all changes made to the streambuffer
  void refreshScreen();

 private:
  short cp(Color col);

  char _lastInput{0};
};

#endif
