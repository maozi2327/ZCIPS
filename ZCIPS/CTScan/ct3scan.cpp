#include "stdafx.h"
#include "ct3scan.h"
#include <functional>
#include <chrono>
#include "linedetnetwork.h"
#include "linedetimageprocess.h"
#include "../Public/util/IULog.h"
#include "../Public/util/Thread.h"
#include "../Public/util/functions.h"
#include "../Public/headers/setupdata.h"
#include "../Public/util/logmacro.h"
#include "simotioncontroller.h"
#include "filedb.h"

CT3Scan::CT3Scan(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork,
	const SetupData* _setupData, int _lineDetIndex, LineDetImageProcess* _lineDetImageProcess)
	: LineDetScanInterface(_controller, _lineDetNetWork, _setupData, _lineDetIndex, _lineDetImageProcess)
{
	
}

CT3Scan::~CT3Scan()
{
	if(d_scanThread.get() != nullptr)
		d_scanThread->stopThread();
}

bool CT3Scan::setScanParameter(std::vector<float> _layer, int _matrix, float _view,	int _sampleTime, float _angle, bool _ecqualLayer)
{
	d_layer = _layer;
	d_matrix = _matrix;
	d_view = _view;
	d_sampleTime = _sampleTime;
	d_angle = _angle;
	d_ecqualLayer = _ecqualLayer;
	return true;
}

//更新进度条
//检查扫描结束
//每间隔固定时间保存一个零时文件，扫描完成后合成
//void CT3Scan::scanThread()
//{
//	if (d_lineDetNetWork->startExtTrigAcquire())
//	{
//		static std::chrono::steady_clock::time_point last_time;
//		last_time = d_start_time = std::chrono::steady_clock::now();
//		caculateParemeterAndSetGenerialFileHeader();
//		sendCmdToControl();
//		d_lineDetNetWork->clearRowList();
//		std::this_thread::sleep_for(std::chrono::seconds(3));
//
//		while (d_deadThreadRun)
//		{
//			auto now = std::chrono::steady_clock::now();
//
//			if (now > last_time + d_intervalForSaveTempFile)
//			{
//				auto listTempHead = d_lineDetNetWork->getRowList();
//				saveTempFile(listTempHead);
//				d_lineDetNetWork->clearRowList();
//				last_time = now;
//			}
//
//			emit(signalGraduationCount(100 * d_lineDetNetWork->getGraduationCount() / d_currentScanTotalSamples));
//
//			if (d_controller->readSaveStatus())
//			{
//				saveFile();
//				break;
//			}
//
//			std::this_thread::sleep_for(std::chrono::milliseconds(1));
//		}
//	}
//	else
//		;
//}

void CT3Scan::sendCmdToControl()
{
	char buf[256];
	CT23ScanCmdData	cmdData;
	cmdData.stsBit.s.czAmountInc1 = d_ictHeader.ScanParameter.InterpolationFlag
		= d_setupData->lineDetData[d_lineDetIndex].StandartInterpolationFlag;
	cmdData.stsBit.s.currentLayer = 1;
	cmdData.stsBit.s1.physiInterpolation = d_setupData->lineDetData[d_lineDetIndex].PhysicsInterpolationFlag;
	cmdData.stsBit.s.thirdGeneration = 1;
	cmdData.interpolationAmount = (BYTE)d_ictHeader.ScanParameter.NumberOfInterpolation;
	cmdData.projectionAmount = d_ictHeader.ScanParameter.Pixels = d_matrix;
	d_ictHeader.ScanParameter.SampleTime = float(d_sampleTime / 1000);
	cmdData.sampleTime = d_sampleTime;
	cmdData.viewDiameter = d_ictHeader.ScanParameter.ViewDiameter = d_view;
	cmdData.orientInc = (short)d_ictHeader.ScanParameter.Azimuth;
	cmdData.ct2Mode = 0;
	cmdData.sliceAmount = d_ictHeader.ScanParameter.TotalLayers;
	cmdData.tabelNumber = 0;
	cmdData.reserved1 = 0;
	cmdData.centerOffset = 0;
	cmdData.stsBit.s.btnStartScan = 0;
	cmdData.stsBit.s.autoStopBeam = 0;
	int dataSize = sizeof(cmdData);

	if (d_layer.size() == 1 || d_ecqualLayer)
	{
		cmdData.stsBit.s.changeLayerSpace = 0;

		if (!d_ecqualLayer)
		{
			cmdData.layerSpace = 0;
			cmdData.stsBit.s.currentLayer = 1;
		}
		else
		{
			cmdData.layerSpace = d_layer[1] - d_layer[0];
			cmdData.stsBit.s.currentLayer = 0;
		}

		cmdData.firstLayerOffset = d_layer[0];
		memcpy(buf, &cmdData, sizeof(cmdData));
	}
	else
	{
		cmdData.stsBit.s.changeLayerSpace = 1;
		cmdData.stsBit.s.currentLayer = 0;
		memcpy(buf, &cmdData, sizeof(cmdData));
		int i = 0;

		for (auto layer : d_layer)
			*(float*)(buf + sizeof(cmdData) - 2 * sizeof(float) + (i++) * sizeof(float)) = layer;

		dataSize = dataSize - 2 * sizeof(float) + d_layer.size() * sizeof(float);
	}

	d_controller->sendToControl(CMD_CT_SCAN, buf, dataSize, false);
}

void CT3Scan::saveFile()
{
	saveOrgFile();

	QDir dir;

	if (!dir.exists(d_filePath))
		dir.mkpath(d_filePath);

	QFile::copy(d_airFile, d_installDirectory + "air.dat");
	d_lineDetImageProcess->ct3Tune(d_orgName, d_filePath);
	++d_imageScaned;	
	
	if (!d_saveOrg)
		QFile::remove(d_orgName);
}

bool CT3Scan::caculateParemeterAndSetGenerialFileHeader()
{
	LineDetScanInterface::caculateParemeterAndSetGenerialFileHeader();

	d_ictHeader.ScanParameter.SampleTime = float(d_sampleTime) / 1000;
	d_ictHeader.ScanParameter.SetupSynchPulseNumber
		= (WORD)(d_ictHeader.ScanParameter.SampleTime * d_ictHeader.SystemParameter.SynchFrequency);
	d_ictHeader.ScanParameter.NumberOfGraduation = d_matrix;
	d_ictHeader.ScanParameter.Azimuth = d_angle;
	d_ictHeader.ScanParameter.NumberofValidVerticalDetector = d_channelNum;
	//对无关参数设置默认值
	d_ictHeader.ScanParameter.ViewDiameter = d_viewDiameter;
	d_ictHeader.ScanParameter.Pixels = d_matrix;
	d_ictHeader.ScanParameter.InterpolationFlag = d_setupData->lineDetData[d_lineDetIndex].StandartInterpolationFlag;
	d_ictHeader.ScanParameter.ScanMode = static_cast<char>(ScanMode::CT3_SCAN);
	d_ictHeader.ScanParameter.TotalLayers = d_layer.size();
	d_currentScanTotalSamples = (d_ictHeader.ScanParameter.NumberOfInterpolation + 1) * d_matrix;
	d_allScanTotalSamples = d_currentScanTotalSamples * d_layer.size();
	int N = d_ictHeader.ScanParameter.NumberOfSystemHorizontalDetector;
	float d = PI * d_ictHeader.ScanParameter.HorizontalSectorAngle / (180 * (N - 1));
	d *= d_ictHeader.ScanParameter.SourceDetectorDistance;
	N = d_ictHeader.ScanParameter.NumberOfInterpolation;

	if ((d / N < d_setupData->minInterpolationSpace) || (N > 80)) 
	{
		LOG_ERROR("插值次数太多！视场直径%.0f, 图像矩阵%d", d_view, d_matrix);
		return false;
	}

	return true;
}

bool CT3Scan::canScan()
{
	QString str;

	if(!LineDetScanInterface::canScan())
		return false;
	//TODO_DJ:
	//if (!QFile::exists(d_airFile))
	//	return false;

	return true;
}

bool CT3Scan::scanFinished()
{
	if(d_imageScaned == d_layer.size())
		return true;

	return false;
}

void CT3Scan::saveTempFile(LineDetList* _listHead)
{
	auto listItem = _listHead;
	auto listItemNum = d_lineDetNetWork->getListItemNum();
	auto listItemSize = d_lineDetNetWork->getListItemSize();
	char* fileMemory = new char(listItemNum * listItemSize);
	int itemCount = 0;

	while (listItem->d_next != nullptr)
	{
		memcpy(fileMemory + itemCount * listItemSize, listItem->d_item, listItemSize);
		++itemCount;
	}

	QString tempFileName = d_tempFileDir + '/' + QString::number(++d_tempFileNum);
	QFile file;
	file.setFileName(tempFileName);
	file.open(QIODevice::WriteOnly);
	file.write(fileMemory, listItemNum * listItemSize);
	file.flush();
	file.close();
	delete[] fileMemory;
	d_tempFileVec.push_back(tempFileName);
}
