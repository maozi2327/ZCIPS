#include "stdafx.h"
#include "drscan.h"


DrScan::DrScan(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork, 
	const SetupData* _setupData, int _lineDetIndex)
	: LineDetScanInterface(_controller, _lineDetNetWork, _setupData, _lineDetIndex)
{
}


DrScan::~DrScan()
{
}
