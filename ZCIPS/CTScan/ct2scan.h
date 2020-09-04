#pragma once
#include "linedetscaninterface.h"
class Ct2Scan :
	public LineDetScanInterface
{
public:

	Ct2Scan(ControllerInterface* in_controller, LineDetNetWork* in_lineDetNetWor, 
		const SetupData* in_setupData, int in_lineDetIndex);
	~Ct2Scan();
};

