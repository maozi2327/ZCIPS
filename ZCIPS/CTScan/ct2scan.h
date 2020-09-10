#pragma once
#include "linedetscaninterface.h"
class Ct2Scan :
	public LineDetScanInterface
{
public:

	Ct2Scan(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWor, 
		const SetupData* _setupData, int _lineDetIndex);
	~Ct2Scan();
};

