#include "statusBar.h"

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
	string more = "..";

	for (;;)
	{

		int addChars = 1;	// space
		if (_messages.size() > 1)
			addChars += more.length();

		if (_messages.empty())
		{
			return line;
		}
		else if (line.length() + _messages.front().length() + addChars < maxLen)
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
