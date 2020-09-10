#pragma once
#include "linedetscaninterface.h"
class DrScan :
	public LineDetScanInterface
{
public:	
	DrScan(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWor, 
		const SetupData* _setupData, int _lineDetIndex);
	~DrScan();
};

