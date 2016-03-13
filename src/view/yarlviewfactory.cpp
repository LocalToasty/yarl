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

#include "yarlviewfactory.h"
#include "yarlconfig.h"

#if USE_SDL == ON
#include "sdlyarlview.h"
#else
#include "cursesyarlview.h"
#endif

std::unique_ptr<YarlView> makeView(YarlController& controller, World& world) {
#if USE_SDL == ON
  return std::make_unique<SDLYarlView>(controller, world);
#else
  return std::make_unique<CursesYarlView>(controller, world);
#endif
}
