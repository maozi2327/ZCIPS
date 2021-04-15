#include "stdafx.h"
#include "conejointscan.h"
#include "controllerinterface.h"
#include "imageprocess.h"
#include "../../Public/util/functions.h"
#include "../PanelDetector/panel.h"


//锥束拼接扫描只是依靠视场大小进行扫描而不提供在任意位置进行扫描的原因是，拼接扫描需要一定的重合区域，在任意位置进行
//扫描，依靠操作人员判断视场能否包下工件无法确定重叠区域
ConeJointScan::ConeJointScan(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _ctTune, bool _bkgTuneFlag,
	bool _airTuneFlag, bool _defectTuneFlag, PanDetData _pandetData, const QString& _airFileNameA, const QString& _airFileNameB)
	: ConeScanInterface(_panel, _controller, _ctTune, _bkgTuneFlag, _airTuneFlag, _defectTuneFlag, _pandetData)
	, d_sampleCount(0), d_airFileNameA(_airFileNameA), d_airFileNameB(_airFileNameB)
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
		printf("%d\n", d_scanProc);

		switch (d_scanProc) 
		{
		case 0:
			//延时1s，因为上面一步发出onstartnextscan后，ctrlSysSts.s.waitNextScan可能还没有更新
			//还处在waitNextScan为true等待下位机发送定位到下个扫描位置阶段，而不是等待启动旋转阶段，因此延时等待下位机更新后继续
			//如果下位机定位到扫描位置，waitNextScan为true,下位机延时2s后开始旋转扫描采集，
			//避免图片还没处理完就更新了文件名
			if (d_controller->readWaitNextScanStatus())
			{
				if (d_sampleCount == 0)
					d_imageProcess->loadAirData(d_airFileNameA);
				else if(d_sampleCount == 1)
					d_imageProcess->loadAirData(d_airFileNameB);

				d_imageReceivedCountThisRound = 0;
				d_panel->beginExTriggerAcquire(frameCallback, d_cycleTime, 2);
				d_scanProc = 8;
			}
			break;
		case 8:
			d_controller->startNextScan();
			std::this_thread::sleep_for(std::chrono::seconds(2));
			d_scanProc = 12;
			break;
		case 12:
			
			if (d_controller->readSaveStatus())
			{
				d_sampleCount++;
				d_panel->stopAcquire();
				d_scanProc = 14;
			}

			break;
		case 14:
			
			if(d_sampleCount != d_round)
				d_scanProc = 0;
			else if (d_imageList.size() == 0)
				//校正保存完成后才开始拼接，而不是下位机发送保存命令后就开始，避免还没有校正保存完成就拼接，导致拼接失败
				d_scanProc = 16;

			break;
		case 16:
			std::this_thread::sleep_for(std::chrono::seconds(2));
			d_panel->stopAcquire();
			std::thread([this](QString& _a, QString& _b,
				QString& _c, int& _pro) { d_imageProcess->dataSplice(_a, _b, _c, _pro); }, d_pathA, d_pathB, d_pathDest, std::ref(d_spliceProgress)).detach();
			d_scanProc = 20;
			break;
		case 20:
			QString spliceProgress = QString::number(d_spliceProgress);
			spliceProgress = QString::fromLocal8Bit("拼接进度") + spliceProgress + QString::fromLocal8Bit("%");
			emit scanProgressSignal(d_imageReceivedCountThisRound, d_graduation, 
				d_graduationCount, d_round * d_graduation * d_framesPerGraduation, spliceProgress);
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

//前提是不能丢图
void ConeJointScan::createFileName(QString& _orgFileName, QString& _tunedFileName)
{
	QDir dir;

	if (d_imageReceivedCountThisRound == 0)
	{
		QString orgPath = d_orgName.left(d_orgName.lastIndexOf('/') + 1);
		d_pureDirectoryName = d_timeName + d_orgName.right(d_orgName.length() - d_orgName.lastIndexOf('_')) + QString::fromLocal8Bit("/");
		QDir dir;

		if (d_sampleCount == 0)
		{
			d_currentOrgPath = d_pathA = orgPath + d_pureDirectoryName + QString::number(0) + QString::fromLocal8Bit("/");
			d_currentTunedPath = d_tunedFilePath + d_pureDirectoryName + QString::number(0) + QString::fromLocal8Bit("/");
		}
		else
		{
			d_currentOrgPath = d_pathB = orgPath + d_pureDirectoryName + QString::number(1) + QString::fromLocal8Bit("/");
			d_pathDest = d_tunedFilePath + d_pureDirectoryName + QString::fromLocal8Bit("joint") + QString::fromLocal8Bit("/");
			dir.mkpath(d_pathDest);
			d_currentTunedPath = d_tunedFilePath + d_pureDirectoryName + QString::number(1) + QString::fromLocal8Bit("/");
		}

		dir.mkpath(d_currentOrgPath);
		makeParameterText();
		writeParameterFile(d_currentOrgPath);
		dir.mkpath(d_currentTunedPath);
		writeParameterFile(d_currentTunedPath);
	}
	
	QString index;
	index.sprintf("%04d", d_imageReceivedCountThisRound);
	_orgFileName = d_currentOrgPath + index + QString::fromLocal8Bit(".tif");
	_tunedFileName = d_currentTunedPath + index + QString::fromLocal8Bit(".tif");
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

bool ConeJointScan::makeParameterText()
{
	ConeScanInterface::makeParameterText();
	auto itr = std::find(d_parameterText.begin(), d_parameterText.end(), QString::fromLocal8Bit("[SCANPARA]\n"));
	QString string = QString::fromLocal8Bit("ScanMode=锥束面阵拼接扫描 \t\t\t;扫描方式\n");
	d_parameterText.insert(++itr, string);
	itr = std::find(d_parameterText.begin(), d_parameterText.end(), QString::fromLocal8Bit("[GEOMETRY]\n"));
	string = makeFormatQString("Translation=%.3f\t\t\t;转台旋转加平移拼接扫描平移距离\n", d_detTransSpace);
	d_parameterText.insert(++itr, string);
	return false;
}

bool ConeJointScan::beginScan(int _graduation, int _framesPerGraduation, int _round, int _posTime, int _cycleTime,
	unsigned short _gainFactor, float _orientInc, float _slicePos, float _sod, float _sdd, float _detTranStart, float _detTranSpace)
{
	setCommonScanParameter(_graduation, _framesPerGraduation, _round, _posTime, _cycleTime, _gainFactor, _orientInc, _slicePos, _sod, _sdd);
	d_detTransStart = _detTranStart;
	d_detTransSpace = _detTranSpace;
	d_sampleCount = 0;
	d_imageReceivedCountThisRound = 0;
	sendCmdToController();
	detachScanProcessThread();
	d_timeName = getTimeWithUnderline();
	return true;
}




















