#include "stdafx.h"
#include "ct2scan.h"

Ct2Scan::Ct2Scan(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWor, 
	const SetupData* _setupData, int _lineDetIndex)
	: LineDetScanInterface(_controller, _lineDetNetWor, _setupData, _lineDetIndex)
{
}

Ct2Scan::~Ct2Scan()
{
}
