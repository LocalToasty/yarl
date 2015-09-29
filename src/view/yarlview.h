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

#ifndef YARLVIEW_H
#define YARLVIEW_H

#include <string>
#include <list>
#include <functional>
#include <vector>
#include <boost/optional.hpp>

class Item;

enum class Color { black, red, green, yellow, blue, magenta, cyan, white };

class YarlView {
 public:
  virtual ~YarlView() = 0;

  virtual void run() = 0;
  virtual void quit() = 0;

  virtual bool yesNoPrompt(std::string query, bool defAnswer) = 0;

  /*!
   * \brief Prompts the user to choose an entry from a list of possible answers.
   * \param possibleAnswers a list of possible answers.
   * \param defAnswer the index of the default answer.
   * \returns the index of the chosen answer, or defAnswer if no choice was
   * made.
   */
  virtual int multipleChoiceDialog(
      std::string const& query, std::vector<std::string> const& possibleAnswers,
      size_t defAnswer) = 0;

  /*!
   * \brief Prompts the user to select an item from a range.
   *
   * \param message	message shown to the user
   * \param pred	the predicate after which the selectable elements are
   *    obtained.
   *		The user is only shown items for which the predicate holds true.
   *
   * The function displays the user a list containing the items in the range
   * [first, last) for * which the predicate holds true, from which he can
   * select one.
   *
   * \returns a pointer to the selected Item, or none if no item was chosen.
   */
  virtual Item* promptItem(std::string const& message,
                           std::list<Item*>::iterator first,
                           std::list<Item*>::iterator last,
                           std::function<bool(Item*)> pred) = 0;

  /*!
   * \brief Prompts the user for a location, usually by letting him select a
   *   tile with a cursor.
   * \returns a pair containing the locations coordinates.
   */
  virtual boost::optional<std::pair<int, int>> promptCoordinates() = 0;

  virtual void showItemList(std::string const& title,
                            std::list<Item*> const& items,
                            std::function<std::string(Item*)> decorate) = 0;

  virtual void addStatusMessage(std::string const& message) = 0;
};

#endif
