#include "stdafx.h"
#include "controllerinterface.h"


ControllerInterface::ControllerInterface(std::map<Axis, AxisData>& _axisDataMap)
	: d_timeoutCount(0), d_connected(false), d_axisDataMap(_axisDataMap)
{

}


ControllerInterface::~ControllerInterface()
{

}
