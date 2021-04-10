#include "stdafx.h"
#include "ConeScan.h"
#include "../PanelImageProcess/panelimageprocess.h"
#include "../Public/headers/setupdata.h"
#include "controllerinterface.h"
#include "../PanelDetector/panel.h"


ConeScan::ConeScan(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _ctTune, bool _bkgTuneFlag,
	bool _airTuneFlag, bool _defectTuneFlag, PanDetData _pandetData) :
	ConeScanInterface(_panel, _controller, _ctTune, _bkgTuneFlag, _airTuneFlag, _defectTuneFlag, _pandetData)
{

}

ConeScan::~ConeScan()
{

}

void ConeScan::frameProcessCallback(unsigned short * _image)
{
	ConeScanInterface::frameProcessCallback(_image);
}

bool ConeScan::stopScan()
{
	return false;
}

void ConeScan::scanThread()
{
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::function<void(unsigned short *)> frameCallback = std::bind(
		&ConeScan::frameProcessCallback, this, std::placeholders::_1);

	while (d_scanThreadRun)
	{
		switch (d_scanProc) 
		{
		case	0:
			if (d_controller->readWaitNextScanStatus()) 
			{
				d_panel->beginExTriggerAcquire(frameCallback, d_cycleTime, 2);
				d_scanProc = 10;
			}
			break;
		case	10:
			d_controller->startNextScan();
			d_scanProc = 1;
			break;
		case    1:
			if (d_controller->readSaveStatus())
			{
				d_scanProc = 2;
			}
			break;
		case    2:
			if (d_graduationCount >= d_graduation)
			{
				d_panel->stopAcquire();
				d_scanProc = 3;
			}
			break;
		default:
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

bool ConeScan::intialise()
{
	return false;
}

void ConeScan::sendCmdToController()
{
	ConeScanCmdData	cmdData;
	cmdData.stsBit.s.translationCone = 0;
	cmdData.stsBit.s.currentLayer = 1;
	cmdData.stsBit.s.coneHelix = 0;
	cmdData.projectionAmount = d_framesPerGraduation * d_graduation;
	cmdData.sampleTime = d_posTime;
	cmdData.orientInc = d_orientInc;
	cmdData.circleAmount = 1;
	cmdData.centerOffset = 0;
	cmdData.firstLayerOffset = d_slicePos;
	cmdData.helixSpace = 0;
	cmdData.b180Scan = 0;
	cmdData.centerOffset = 0;
	cmdData.stsBit.s.coneHelix = 0;
	d_controller->sendToControl(CMD_CONE_SCAN, (char*)(&cmdData), sizeof(ConeScanCmdData), false);
}
