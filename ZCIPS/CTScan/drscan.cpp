#include "stdafx.h"
#include "drscan.h"


DrScan::DrScan(ControllerInterface* in_controller, LineDetNetWork* in_lineDetNetWork, 
	const SetupData* in_setupData, int in_lineDetIndex)
	: LineDetScanInterface(in_controller, in_lineDetNetWork, in_setupData, in_lineDetIndex)
{
}


DrScan::~DrScan()
{
}
