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
