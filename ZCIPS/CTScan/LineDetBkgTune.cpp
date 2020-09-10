#include "stdafx.h"
#include "linedetbkgtune.h"
#include "controllerinterface.h"
#include "../Public/util/Thread.h"
#include "LineDetImageProcess.h"
#include "LineDetNetWork.h"

LineDetBkgTune::LineDetBkgTune(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork,
	const SetupData* _setupData, int _lineDetIndex)
	: LineDetScanInterface(_controller, _lineDetNetWork, _setupData, _lineDetIndex)
{
}


LineDetBkgTune::~LineDetBkgTune()
{
}
