#include "stdafx.h"
#include "ct2scan.h"

Ct2Scan::Ct2Scan(ControllerInterface* in_controller, LineDetNetWork* in_lineDetNetWor, 
	const SetupData* in_setupData, int in_lineDetIndex)
	: LineDetScanInterface(in_controller, in_lineDetNetWor, in_setupData, in_lineDetIndex)
{
}

Ct2Scan::~Ct2Scan()
{
}
