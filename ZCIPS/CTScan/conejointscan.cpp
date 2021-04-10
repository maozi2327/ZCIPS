#include "stdafx.h"
#include "conejointscan.h"
#include "controllerinterface.h"
#include "imageprocess.h"
#include "../../Public/util/functions.h"
#include "../PanelDetector/panel.h"


ConeJointScan::ConeJointScan(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _ctTune, bool _bkgTuneFlag,
	bool _airTuneFlag, bool _defectTuneFlag, PanDetData _pandetData) 
	: ConeScanInterface(_panel, _controller, _ctTune, _bkgTuneFlag, _airTuneFlag, _defectTuneFlag, _pandetData)
	, d_sampleCount(0)
{

}


ConeJointScan::~ConeJointScan()
{
}

void ConeJointScan::scanThread()
{
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::function<void(unsigned short *)> frameCallback = std::bind(
		&ConeJointScan::frameProcessCallback, this, std::placeholders::_1);
	std::thread spliceThread;

	while (d_scanThreadRun)
	{
		switch (d_scanProc) 
		{
		case 4:
			//延时1s，因为上面一步发出onstartnextscan后，ctrlSysSts.s.waitNextScan可能还没有更新
			//还处在waitNextScan为true等待下位机发送定位到下个扫描位置阶段，而不是等待启动旋转阶段，因此延时等待下位机更新后继续
			//如果下位机定位到扫描位置，waitNextScan为true,下位机延时2s后开始旋转扫描采集，
			if (d_controller->readReadyStatus()) 
			{
				d_panel->beginExTriggerAcquire(frameCallback, d_cycleTime, 2);
				d_scanProc = 20;
			}
			break;
		case 8:
			d_controller->startNextScan();
			std::this_thread::sleep_for(std::chrono::seconds(2));
			d_scanProc = 5;
		case 12:
			if (d_controller->readSaveStatus()) 
				d_scanProc = 10;
			break;
		case 14:
			if (d_graduationCount >= d_graduation)
			{
				if (++d_sampleCount == d_round) 
					d_scanProc = 16;
				else 
					d_scanProc = 4;
			}
			break;
		case 16:
			std::this_thread::sleep_for(std::chrono::seconds(2));
			d_panel->stopAcquire();

			spliceThread = 	std::thread(
				std::bind(&PanelImageProcess::dataSplice, std::placeholders::_1, std::placeholders::_1, std::placeholders::_1, std::placeholders::_1),
				d_pathA, d_pathB, d_pathDest, std::ref(d_spliceProgress));

			spliceThread.detach();
			d_scanProc = 20;
			break;
		case 20:
			QString spliceProgress = QString::number(d_spliceProgress);
			spliceProgress = QString::fromLocal8Bit("拼接进度") + spliceProgress + QString::fromLocal8Bit("%");
			emit scanProgressSignal(d_graduationCount, d_graduation, d_graduationCount, d_round * d_graduation * d_framesPerGraduation, spliceProgress);
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void ConeJointScan::createFileName(QString& _orgFileName, QString& _tunedFileName)
{
	if (d_graduationCount % d_graduation == 0)
	{
		QString orgPath = d_orgName.left(d_orgName.lastIndexOf('/') + 1);
		QString timeName = getTimeWithUnderline();
		d_pureDirectoryName = timeName + d_orgName.right(d_orgName.length() - d_orgName.lastIndexOf('_')) + QString::fromLocal8Bit("/");
		QDir dir;
		QString currentOrgPath, currentTunedPath;

		if (d_sampleCount == 0)
		{
			currentOrgPath = d_pathA = orgPath + d_pureDirectoryName + QString::number(d_sampleCount) + QString::fromLocal8Bit("/");
		}
		else if (d_sampleCount == 1)
		{
			currentOrgPath = d_pathB = orgPath + d_pureDirectoryName + QString::number(d_sampleCount) + QString::fromLocal8Bit("/");
		}

		dir.mkpath(currentOrgPath);
		makeParameterText();
		writeParameterFile(currentOrgPath);
		currentTunedPath = d_tunedFilePath + d_pureDirectoryName + QString::number(d_sampleCount) + QString::fromLocal8Bit("/");
		dir.mkpath(currentTunedPath);
		writeParameterFile(currentTunedPath);
	}

	QString index;
	index.sprintf("%04d", int(d_graduationCount % d_graduation));
	_orgFileName = d_orgName + d_pureDirectoryName + QString::number(d_sampleCount) + QString::fromLocal8Bit("/") + index + QString::fromLocal8Bit(".tif");
	_tunedFileName = d_tunedFilePath + d_pureDirectoryName + QString::number(d_sampleCount) + QString::fromLocal8Bit("/") + index + QString::fromLocal8Bit(".tif");
}

void ConeJointScan::sendCmdToController()
{
	ConeJointScanCmdData cmdData;
	cmdData.stsBit.s.translationCone = 0;
	cmdData.stsBit.s.currentLayer = 1;
	cmdData.stsBit.s.coneHelix = 0;
	cmdData.projectionAmount = d_framesPerGraduation * d_graduation;
	cmdData.sampleTime = d_posTime;
	cmdData.orientInc = d_orientInc;
	cmdData.circleAmount = 1;
	cmdData.centerOffset = 0;
	cmdData.firstLayerOffset = d_slicePos;
	cmdData.objRadialPos = d_sod;
	cmdData.detRadialPos = d_sdd;
	cmdData.detTransStart = d_detTransStart;
	cmdData.detTransSpace = d_detTransSpace;
	cmdData.b180Scan = 0;
	cmdData.centerOffset = 0;
	cmdData.detTransTimes = 1;
	cmdData.bSkipAirTune = 1;
	cmdData.stsBit.s.coneHelix = 0;
	d_controller->sendToControl(CMD_CONEJOINT_SCAN, (char*)(&cmdData), sizeof(ConeJointScanCmdData), false);
}
