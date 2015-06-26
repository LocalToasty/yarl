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

#include "statusbar.h"

bool StatusBar::empty()
{
	return _messages.empty();
}

void StatusBar::addMessage(std::string message)
{
	_messages.push_back(message);
}

string StatusBar::getLine(int maxLen)
{
	string line;
	string more = ".. ";

	for (;;)
	{

		int addChars = 2;	// preceding + leading space
		if (_messages.size() > 1)
			addChars += more.length();

		if (_messages.empty())
		{
			line.push_back(' ');
			return line;
		}
		else if(line.length() + _messages.front().length() + addChars <  maxLen)
		{
			line.push_back(' ');
			line.append(_messages.front());
			_messages.pop_front();
		}
//		else if (line.empty())	// TODO
		else
		{
			return line.append(more);
		}

	}
}
