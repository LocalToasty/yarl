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

#ifndef SDLYARLVIEW_H
#define SDLYARLVIEW_H

#include "consoleyarlview.h"
#include "statusbar.h"
#include "vec.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>
#include <limits>

class YarlController;
class World;
class Item;

class SDLYarlView : public ConsoleYarlView {
 public:
  SDLYarlView(YarlController& controller, World& world);
  ~SDLYarlView();

 protected:
  char getChar();
  void waitForInput();

  // width of the terminal screen
  int width() const;
  // height of the terminal screen
  int height() const;

  // turn the cursor on or off
  void cursor(bool val) { _cursorOn = val; }

  // cursor coordinates
  Vec<int, 2> cursorPos() const { return _cursPos; }

  // replace the character at the current cursor location
  void addChar(char c, Color col = Color::white);
  // write a string of characters
  void addString(std::string s, Color col = Color::white);

  // move the cursor to the given coordinates
  void moveCursor(Vec<int, 2> pos);

  void clear(int x, int y, int w, int h);

  // show all changes made to the streambuffer
  void refreshScreen();

 private:
  int const static _defaultWidth;
  int const static _defaultHeight;

  SDL_Window* _window;
  SDL_Surface* _charset;

  std::vector<char> _characters;
  std::vector<Color> _colors;

  // character dimensions
  int _charWidth;
  int _charHeight;

  bool _cursorOn;

  std::queue<char> _inputBuffer;

  // screen dimensions (in characters)
  int _width{_defaultWidth};
  int _height{_defaultHeight};

  // cursor position
  Vec<int, 2> _cursPos{{0, 0}};

  bool _useColor;

  Uint32 color(Color col);
};

#endif
