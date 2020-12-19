#include "stdafx.h"
#include "ConeScan.h"
#include "../PanelImageProcess/panelimageprocess.h"
#include "../Public/headers/setupdata.h"
#include "controllerinterface.h"
#include "../PanelDetector/panel.h"


ConeScan::ConeScan(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _ctDispose) :
	ConeScanInterface(_panel, _controller, _ctDispose)
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
	while (d_deadThreadRun)
	{
		if (d_graduationCount >= d_graduation)
		{
			d_panel->stopAcquire();
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

bool ConeScan::loadAirData()
{
	if (!d_imageProcess->loadAirData(QString("air.tif")))
	{
		LOG_ERROR("×¶ÊøÉ¨Ãè¼ÓÔØ¿ÕÆøÎÄ¼þÊ§°Ü£¡");
		return false;
	}

	return true;
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
	cmdData.firstLayerOffset = 400;
	cmdData.helixSpace = 0;
	cmdData.b180Scan = 0;
	cmdData.centerOffset = 0;
	cmdData.stsBit.s.coneHelix = 0;
	d_controller->sendToControl(CMD_CONE_SCAN, (char*)(&cmdData), sizeof(ConeScanCmdData), false);
}
