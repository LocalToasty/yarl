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

#include "sdlyarlview.h"
#include "yarlcontroller.h"
#include "player.h"
#include "command.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <map>

const int SDLYarlView::_defaultWidth = 80;
const int SDLYarlView::_defaultHeight = 50;

/*!
 * \brief Converts a color into RGBA format.
 */
Uint32 SDLYarlView::color(Color col) {
  SDL_Surface* screen = SDL_GetWindowSurface(_window);

  if (!_useColor) {
    return SDL_MapRGB(screen->format, 0xff, 0xff, 0xff);
  }

  switch (col) {
    case Color::black:
      return SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);

    case Color::red:
      return SDL_MapRGB(screen->format, 0xff, 0x00, 0x00);

    case Color::green:
      return SDL_MapRGB(screen->format, 0x00, 0xff, 0x00);

    case Color::yellow:
      return SDL_MapRGB(screen->format, 0xff, 0xff, 0x00);

    case Color::blue:
      return SDL_MapRGB(screen->format, 0x00, 0x00, 0xff);

    case Color::magenta:
      return SDL_MapRGB(screen->format, 0xff, 0xff, 0xff);

    case Color::cyan:
      return SDL_MapRGB(screen->format, 0x00, 0xff, 0xff);

    default:
    case Color::white:
      return SDL_MapRGB(screen->format, 0xff, 0xff, 0xff);
  }
}

SDLYarlView::SDLYarlView(YarlController& controller, World& world)
    : ConsoleYarlView(controller, world), _cursorOn(false), _useColor(true) {
  const char* charset = "charset.bmp";
  _characters = std::vector<char>(_width * _height, ' ');
  _colors = std::vector<Color>(_width * _height, Color::white);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    std::cerr << "Error while initializing SDL: " << SDL_GetError()
              << std::endl;
    return;  // TODO
  }

  SDL_Surface* tmpCharset = SDL_LoadBMP(charset);

  if (tmpCharset == nullptr) {
    std::cerr << "Error while trying to open char set: " << SDL_GetError()
              << std::endl;
    return;  // TODO
  }

  _charWidth = tmpCharset->w / 16;
  _charHeight = tmpCharset->h / 8;

  _window =
      SDL_CreateWindow("yarl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       _defaultWidth * _charWidth, _defaultHeight * _charHeight,
                       SDL_WINDOW_SHOWN);

  if (_window == nullptr) {
    std::cerr << "Error while opening window: " << SDL_GetError() << std::endl;
    return;  // TODO
  }

  SDL_Surface* screen = SDL_GetWindowSurface(_window);

  // optimise charset format
  _charset = SDL_ConvertSurface(tmpCharset, screen->format, 0);
  SDL_FreeSurface(tmpCharset);
}

SDLYarlView::~SDLYarlView() {
  SDL_DestroyWindow(_window);
  SDL_FreeSurface(_charset);

  SDL_Quit();
}

char SDLYarlView::getChar() {
  while (_inputBuffer.empty()) {
    waitForInput();
  }

  char ret = _inputBuffer.front();
  _inputBuffer.pop();

  return ret;
}

int SDLYarlView::width() const { return _width; }

int SDLYarlView::height() const { return _height; }

void SDLYarlView::addChar(char c, Color col) {
  _characters.at(_cursPos[0] + _width * _cursPos[1]) = c;
  _colors.at(_cursPos[0] + _width * _cursPos[1]) = col;

  _cursPos[0]++;

  if (_cursPos[0] > _width) {
    _cursPos[0] = 0;
    _cursPos[1]++;
  }
}

void SDLYarlView::addString(std::string s, Color col) {
  for (char c : s) {
    addChar(c, col);
  }
}

void SDLYarlView::moveCursor(Vec<int, 2> pos) {
  if (pos[0] >= _width) {
    _cursPos[0] = _width;
  } else if (pos[0] < 0) {
    _cursPos[0] = 0;
  } else {
    _cursPos[0] = pos[0];
  }

  if (pos[1] >= _height) {
    _cursPos[1] = _height;
  } else if (pos[1] < 0) {
    _cursPos[1] = 0;
  } else {
    _cursPos[1] = pos[1];
  }
}

void SDLYarlView::waitForInput() {
  SDL_Event e;

  while (true) {  // getChar has to be blocking
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        //_controller.quit();
      } else if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_RETURN) {
          _inputBuffer.push('\n');
        } else {
          _inputBuffer.push((char)e.key.keysym.sym);
        }

        return;
      }
    }

    SDL_Delay(20);  // to lower CPU usage
  }
}

void SDLYarlView::clear(int x, int y, int w, int h) {
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      _characters.at(x + i + (y + j) * _width) = ' ';
    }
  }
}

void SDLYarlView::refreshScreen() {
  SDL_Surface* screen = SDL_GetWindowSurface(_window);

  for (int y = 0; y < _height; y++) {
    for (int x = 0; x < _width; x++) {
      char c = _characters.at(x + y * _width);
      Color col = _colors.at(x + y * _width);

      SDL_Rect ch;
      ch.x = _charWidth * (c & 0x0f);
      ch.y = _charHeight * ((c & 0x70) >> 4);
      ch.w = _charWidth;
      ch.h = _charHeight;

      SDL_Rect pos;
      pos.x = _charWidth * x;
      pos.y = _charHeight * y;
      pos.w = _charWidth;
      pos.h = _charHeight;

      if (_cursorOn && _cursPos[0] == x && _cursPos[1] == y) {
        SDL_SetColorKey(_charset, SDL_TRUE,
                        SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_FillRect(screen, &pos,
                     SDL_MapRGB(screen->format, 0x80, 0x80, 0x80));
      } else {
        SDL_SetColorKey(_charset, SDL_TRUE,
                        SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));
        SDL_FillRect(screen, &pos, color(col));
      }

      SDL_BlitSurface(_charset, &ch, screen, &pos);
    }
  }

  SDL_UpdateWindowSurface(_window);
}
