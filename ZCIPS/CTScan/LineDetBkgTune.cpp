#include "stdafx.h"
#include "linedetbkgtune.h"
#include "controllerinterface.h"
#include "../Public/util/Thread.h"
#include "LineDetImageProcess.h"
#include "LineDetNetWork.h"

LineDetBkgTune::LineDetBkgTune(ControllerInterface* in_controller, LineDetNetWork* in_lineDetNetWork,
	const SetupData* in_setupData, int in_lineDetIndex)
	: LineDetScanInterface(in_controller, in_lineDetNetWork, in_setupData, in_lineDetIndex)
{
}


LineDetBkgTune::~LineDetBkgTune()
{
}
