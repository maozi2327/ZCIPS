#pragma once
#include "LineDetScanInterface.h"

class ControllerInterface;
class LineDetBkgTune :
	public LineDetScanInterface
{
public:
	LineDetBkgTune(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork,
		const SetupData* _setupData, int _lineDetIndex);
	~LineDetBkgTune();
};

