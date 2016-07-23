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

#ifndef DROPEVENT_H
#define DROPEVENT_H

#include <memory>
#include "event.h"

class Item;
class Entity;

struct DropEvent : public Event {
  DropEvent(std::shared_ptr<Entity const> dropper,
            std::shared_ptr<Item const> droppedItem)
      : dropper(dropper), item(droppedItem) {}

  std::shared_ptr<Entity const> dropper;
  std::shared_ptr<Item const> item;
};

#endif
