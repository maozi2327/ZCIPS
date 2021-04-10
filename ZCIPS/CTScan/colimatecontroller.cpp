#include "StdAfx.h"
#include "leisaibac332e.h"
#include "ColimateController.h"
#include <thread>

const int AXISFRONTHORIZONTAL = 0;
const int AXISFRONTVERTICAL = 1;
const int AXISPANELHORIZONTAL = 2;
const int AXISPANELVERTICAL = 3;
const int AXISLINETHICKNESS = 3;

ColimateController::ColimateController(const QString& _moduleFileDirectory) : leiSaiMotor(new LeiSaiBac332E(_moduleFileDirectory))
{

}

ColimateController::~ColimateController(void)
{
}

bool ColimateController::switchFrontVerticalToPanel()
{
	return leiSaiMotor->aixAbsMove(AXISFRONTVERTICAL, 10000);
}

bool ColimateController::switchFrontHorizontalToPanel()
{
	return leiSaiMotor->aixAbsMove(AXISFRONTHORIZONTAL, 10000);
}

bool ColimateController::switchFrontVerticalToLine()
{
	return leiSaiMotor->aixAbsMove(AXISFRONTVERTICAL, 15.0 / 2);
}

bool ColimateController::switchFrontHorizontalToLine()
{
	return leiSaiMotor->aixAbsMove(AXISFRONTHORIZONTAL, 10000);
}

bool ColimateController::changeFrontVerticalWidth(float _width)
{
	return leiSaiMotor->aixAbsMove(1, _width / 2);
}

bool ColimateController::changeFrontHorizontalWidth(float _width)
{
	return leiSaiMotor->aixAbsMove(0, _width / 2);
}

bool ColimateController::switchPanelHorizontalToPanel()
{
	return leiSaiMotor->aixAbsMove(AXISPANELHORIZONTAL, 10000);
}

bool ColimateController::switchPanelVerticalToPanel()
{
	return leiSaiMotor->aixAbsMove(AXISPANELVERTICAL, 10000);
}

bool ColimateController::switchPanelVerticalToLine()
{
	return leiSaiMotor->aixAbsMove(AXISPANELVERTICAL, -10000);
}

bool ColimateController::switchPanelHorizontalToLine()
{
	return leiSaiMotor->aixAbsMove(AXISPANELHORIZONTAL, -10000);
}

bool ColimateController::changePanelVerticalWidth(float _width)
{
	return leiSaiMotor->aixAbsMove(4, _width / 2);
}

bool ColimateController::changePanelHorizontalWidth(float _width)
{
	return leiSaiMotor->aixAbsMove(3, _width / 2);
}

bool ColimateController::swithcToPanel()
{
	if(leiSaiMotor->getReadyStatus() && leiSaiMotor->getIdleStatus())
	{
		switchPanelVerticalToPanel();
		switchPanelHorizontalToPanel();
		switchFrontVerticalToPanel();
		switchFrontHorizontalToPanel();
		return TRUE;
	}
	return FALSE;
}

bool ColimateController::swithcToLine()
{
	if(leiSaiMotor->getReadyStatus() && leiSaiMotor->getIdleStatus())
	{
		switchFrontVerticalToLine();
		switchFrontHorizontalToLine();
		switchPanelVerticalToLine();
		switchPanelHorizontalToLine();
		changeLineThickness(-2);
		return TRUE;
	}
	return FALSE;
}

bool ColimateController::changeLineThickness(float _thickNess)
{
	return leiSaiMotor->aixAbsMove(4, _thickNess);
}

int ColimateController::getColimateMode()
{
	if(!leiSaiMotor->getReadyStatus())
		return -1;

	LeiSaiStatus status = leiSaiMotor->readStatus();

	if(	status.axesStatus[AXISFRONTHORIZONTAL].elPositive == TRUE &&
		fabs(status.axesStatus[AXISFRONTVERTICAL].position - 15.0 /2) < 0.001 &&
		status.axesStatus[AXISPANELHORIZONTAL].elNegative == TRUE &&
		status.axesStatus[AXISPANELVERTICAL].elNegative == TRUE)
		return 0;

	if(	status.axesStatus[AXISFRONTHORIZONTAL].elPositive == TRUE &&
		status.axesStatus[AXISFRONTVERTICAL].elPositive == TRUE &&
		status.axesStatus[AXISPANELHORIZONTAL].elPositive == TRUE &&
		status.axesStatus[AXISPANELVERTICAL].elPositive == TRUE)
		return 2;

	return -1;
}

bool ColimateController::getColimateSwitching()
{
	return leiSaiMotor->getIdleStatus();
}

bool ColimateController::homeAllAxes()
{
	return leiSaiMotor->homeAllAxes();
}

void ColimateController::homeAndSwitchThread(int lineOrPanel)
{
	leiSaiMotor->homeAllAxes();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	while(homeAndSwitchThreadRun && !leiSaiMotor->getIdleStatus())
		std::this_thread::sleep_for(std::chrono::milliseconds(100));;

	if(!homeAndSwitchThreadRun)
		return;

	if(lineOrPanel == 0)
		swithcToLine();
	else if(lineOrPanel == 1)
		swithcToPanel();
}

void ColimateController::homeAllAxesAndSwitchToLine()
{
	homeAndSwitchThreadRun = true;
	std::thread(std::bind(&ColimateController::homeAndSwitchThread, this, 0)).detach();
}

void ColimateController::homeAllAxesAndSwitchToPanel()
{
	homeAndSwitchThreadRun = true;
	std::thread(std::bind(&ColimateController::homeAndSwitchThread, this, 0)).detach();
}

void ColimateController::stopAll()
{
	leiSaiMotor->stopAllAxes();
	homeAndSwitchThreadRun = false;
}