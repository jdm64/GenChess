#ifndef __CVEP_H__
#define __CVEP_H__

#include "UI.h"

class CVEP : public UI {
public:
	bool moveResults();

	bool gameCmd();

	bool mainCmd();

	void run();
};

#endif
