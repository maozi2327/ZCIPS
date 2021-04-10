#include "stdafx.h"
#include "conecontinusscan.h"
#include "../PanelImageProcess/panelimageprocess.h"
#include "controllerinterface.h"

ConeContinusScan::ConeContinusScan(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _ctTune):
	ConeScanInterface(_panel, _controller, _ctTune)
{
}


ConeContinusScan::~ConeContinusScan()
{
}

//获取d_panel采集的图像，并校正保存
void ConeContinusScan::scanThread(std::promise<bool>& _promise)
{
	_promise.set_value_at_thread_exit(true);

	while (true)
	{
		//unsigned short* imagePtr;
		//d_panel->getHeadImage(&imagePtr);
		//d_graduation++;

		//if (d_isSaveOrg)
		//	saveFile(imagePtr);

		//if (d_isTune)
		//	;//d_ctTune->tune(imagePtr);

		//saveFile(imagePtr);
		//free(imagePtr);
	}
}

bool ConeContinusScan::stopScan()
{
	//d_controller->sendToControl(0, 0);
	return true;
}
