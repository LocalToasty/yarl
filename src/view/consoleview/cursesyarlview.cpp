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

#include "cursesyarlview.h"
#include <curses.h>

short CursesYarlView::cp(Color col) {
  switch (col) {
    case Color::black:
      return COLOR_BLACK;

    case Color::red:
      return COLOR_RED;

    case Color::green:
      return COLOR_GREEN;

    case Color::yellow:
      return COLOR_YELLOW;

    case Color::blue:
      return COLOR_BLUE;

    case Color::magenta:
      return COLOR_MAGENTA;

    case Color::cyan:
      return COLOR_CYAN;

    case Color::white:
    default:
      return COLOR_WHITE;
  }
}

CursesYarlView::CursesYarlView(YarlController& controller, World& world)
    : ConsoleYarlView(controller, world) {
  initscr();
  cbreak();
  noecho();
  curs_set(false);  // no cursor

  // initialize colors
  if (has_colors()) {
    start_color();

    for (int i = 1; i < 8; i++) {
      init_pair(i, i, COLOR_BLACK);
    }
  }
}

CursesYarlView::~CursesYarlView() { endwin(); }

int CursesYarlView::width() const { return getmaxx(stdscr); }

int CursesYarlView::height() const { return getmaxy(stdscr); }

void CursesYarlView::cursor(bool val) { curs_set(val); }

int CursesYarlView::cursorX() const { return getcurx(stdscr); }

int CursesYarlView::cursorY() const { return getcury(stdscr); }

void CursesYarlView::addChar(char c, Color col) {
  attrset(COLOR_PAIR(cp(col)));
  addch(c);
}

void CursesYarlView::addString(string s, Color col) {
  attrset(COLOR_PAIR(cp(col)));
  addstr(s.c_str());
}

void CursesYarlView::moveCursor(int x, int y) { move(y, x); }

char CursesYarlView::getChar() {
  if (_lastInput != 0) {
    char ret = _lastInput;
    _lastInput = 0;

    return ret;
  } else {
    char c = (char)getch();

    switch (c) {
      case 127:
        return '\b';

      default:
        return c;
    }
  }
}

void CursesYarlView::waitForInput() {
  if (_lastInput != 0) {
    _lastInput = getChar();
  }
}

void CursesYarlView::clear(int x, int y, int w, int h) {
  for (int i = 0; i < w; i++) {
    move(y, x + i);

    for (int j = 0; j < h; j++) {
      addch(' ');
    }
  }
}

void CursesYarlView::refreshScreen() { refresh(); }
