#pragma once
#include "linedetscaninterface.h"
class DrScan :
	public LineDetScanInterface
{
public:	
	DrScan(ControllerInterface* in_controller, LineDetNetWork* in_lineDetNetWor, 
		const SetupData* in_setupData, int in_lineDetIndex);
	~DrScan();
};

