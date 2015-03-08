#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>

using namespace std;

class Variable
{
private:
	string	_val;
	string	_def;
	string	_desc;

public:
	Variable(string def = "", string desc = "");

	void	operator=(string val);
	void	operator=(int val);

	string	toString();
	int		toInt();
};

#endif
