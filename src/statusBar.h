#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <string>
#include <list>

using namespace std;

class StatusBar
{
private:
	list<string> _messages;

public:
	bool empty();

	void addMessage(string message);

	string getLine(int maxLen);
};

#endif
