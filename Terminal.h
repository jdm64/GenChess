/*	GenChess, a genesis chess engine
	Copyright (C) 2010, Justin Madru (jdm64@gawab.com)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include "UI.h"

class Terminal : public UI {
private:
	static const string PS[3];
	
	const string *playerString;

	int PT[3];
	
	int *playerType;

public:
	Terminal(const int white, const int black);

	void displayHelp() const;

	bool playerCmd();

	bool mainMenu();

	void run();
};

#endif
