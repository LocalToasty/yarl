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

#include "variable.h"

#include <stdexcept>

Variable::Variable(std::string def, std::string desc) :
	_val(def), _def(def), _desc(desc)

{
}

void Variable::operator=(string val)
{
	_val = val;
}

void Variable::operator=(int val)
{
	_val = to_string(val);
}

// return variable as a string
string Variable::toString()
{
	return _val;
}

// return variable as an integer if possible
int Variable::toInt()
{
	try
	{
		return stoi(_val);
	}
	catch (invalid_argument)	// _val is not an int
	{
		try
		{
			return stoi(_def);
		}
		catch (invalid_argument)	// _def is not an int
		{
			return 0;
		}
	}
}
