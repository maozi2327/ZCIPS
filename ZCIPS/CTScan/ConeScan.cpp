#include "stdafx.h"
#include "ConeScan.h"
#include "../PanelImageProcess/panelimageprocess.h"
#include "../Public/headers/setupdata.h"
#include "controllerinterface.h"


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
	while (d_scanThread)
	{
		if (d_graduationCount >= d_graduation)
			d_scanThread->stopThread();
	}
}

bool ConeScan::loadAirData()
{
	if (!d_imageProcess->loadAirData(QString("air.tif")))
	{
		LOG_ERROR("׶��ɨ����ؿ����ļ�ʧ�ܣ�");
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
	cmdData.sampleTime = d_sampleTime;
	cmdData.orientInc = d_orientInc;
	cmdData.circleAmount = 1;
	cmdData.centerOffset = 0;
	cmdData.firstLayerOffset = 0;
	cmdData.helixSpace = 0;
	cmdData.b180Scan = 0;
	cmdData.centerOffset = 0;
	cmdData.stsBit.s.coneHelix = 0;
	d_controller->sendToControl(CMD_CONE_SCAN, (char*)(&cmdData), sizeof(ConeScanCmdData), false);
}
