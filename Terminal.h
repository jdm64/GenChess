#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include "UI.h"

class Terminal : public UI {
private:
	static const string PS[3];
	
	const string *playerString;

	char PT[3];
	
	char *playerType;

public:
	Terminal(const int white, const int black);

	bool mainMenu();

	bool playerCmd();

	void displayHelp();

	void run();
};

#endif
