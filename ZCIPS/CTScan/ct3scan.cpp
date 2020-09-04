#include "stdafx.h"
#include <functional>
#include <chrono>
#include "ct3scan.h"
#include "linedetnetwork.h"
#include "linedetimageprocess.h"
#include "../Public/util/IULog.h"
#include "../Public/util/Thread.h"
#include "../Public/util/functions.h"
#include "motorcontrolwidget.h"
#include "../Public/headers/setupdata.h"
#include "../Public/util/macro.h"
#include "simotioncontroller.h"

std::chrono::minutes CT3Scan::d_intervalForSaveTempFile = std::chrono::minutes(3);

CT3Scan::CT3Scan(ControllerInterface* in_controller, LineDetNetWork* in_lineDetNetWork,
	const SetupData* in_setupData, int in_lineDetIndex)
	: LineDetScanInterface(in_controller, in_lineDetNetWork, in_setupData, in_lineDetIndex)
{
	
}

CT3Scan::~CT3Scan()
{
	if(d_scanThread.get() != nullptr)
		d_scanThread->stopThread();
}

bool CT3Scan::setScanParameter(float in_layer, int in_matrix, float in_view, 
	int in_sampleTime, float in_angle)
{
	d_layer = in_layer;
	d_matrix = in_matrix;
	d_view = in_view;
	d_sampleTime = in_sampleTime;
	d_angle = in_angle;
	return true;
}

//更新进度条
//检查扫描结束
//每间隔固定时间保存一个零时文件，扫描完成后合成
void CT3Scan::scanThread()
{
	if (d_lineDetNetWork->startExtTrigAcquire())
	{
		int graduation = loadTempFile();

		if (graduation == -1)
		{

			return;
		}
		else
		{

		}

		d_start_time = std::chrono::steady_clock::now();
		static std::chrono::steady_clock::time_point last_time;
		sendCmdToControl();

		while (d_threadRun)
		{
			auto now = std::chrono::steady_clock::now();

			if (now > last_time + d_intervalForSaveTempFile)
			{
				auto listTempHead = d_lineDetNetWork->getRowList();
				d_lineDetNetWork->clearRowList();
				saveTempFile(listTempHead);
				last_time = now;
			}

			emit(signalGraduationCount(d_lineDetNetWork->getGraduationCount()));

			if (d_controller->readSaveStatus())
			{
				saveFile();
				stopScan();
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	else
		;
}


void CT3Scan::sendCmdToControl()
{
	char buf[RTBUF_LEN];
	CT23ScanCmdData	cmdData, *pCmdData;
	cmdData.stsBit.s.changeLayerSpace = 0;
	cmdData.stsBit.s.czAmountInc1 = d_ictHeader.ScanParameter.InterpolationFlag 
		= d_setupData->lineDetData[d_lineDetIndex].StandartInterpolationFlag;
	cmdData.stsBit.s.currentLayer = 0;
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
	cmdData.firstLayerOffset = d_layer;
	cmdData.layerSpace = 0;
	COMM_PACKET* ptr = (COMM_PACKET*)buf;
	ptr->tagHead[0] = 0x55;
	ptr->tagHead[1] = 0xaa;
	ptr->tagHead[2] = 0x5a;
	ptr->typeCode = CMD_CT_SCAN;
	ptr->tagLen = 3 + sizeof(CT23ScanCmdData);
	memcpy(buf + 6, &cmdData, sizeof(cmdData));
	d_controller->sendToControl(buf, 6 + sizeof(CT23ScanCmdData));
}

void CT3Scan::saveFile()
{
	std::unique_ptr<RowList> finalList(new RowList);

	for (auto& tempFile : d_tempFileVec)
	{
		QFile file;
		file.setFileName(tempFile);
		file.open(QIODevice::ReadWrite);
		auto byteArray = file.readAll();
		int listItemNum = byteArray.size() / d_lineDetNetWork->getListItemSize();
		auto listItemSize = d_lineDetNetWork->getListItemSize();
		char* memory = new char[byteArray.size()];
		memcpy(memory, byteArray.data(), byteArray.size());

		for (int i = 0; i != listItemNum; ++i)
			finalList->push_back((unsigned long*)(memory + listItemSize * i));
	}	
	
	saveOrgFile(finalList.get()->getList());
	QString orgFileName(d_orgPath + d_fileName + ".org");
	QString disposedFileName(d_orgPath + d_fileName);
	d_lineDetImageProcess->dispose(d_installDirectory, orgFileName, disposedFileName);
}

bool CT3Scan::beginScan()
{	
	if (canScan())
	{
		d_scanThread.reset(new Thread(std::bind(&CT3Scan::scanThread, this), 
			std::ref(d_threadRun)));
		return d_scanThread->detach();
	}	
	else
		return false;
}

bool CT3Scan::setGenerialFileHeader()
{
	LineDetScanInterface::setGenerialFileHeader();

	d_ictHeader.DataFormat.graduationBase = 0;						//3代扫描起始分度
	d_ictHeader.ScanParameter.ScanMode = static_cast<char>(ScanMode::CT3_SCAN);
	d_ictHeader.ScanParameter.NumberOfGraduation = d_matrix;
	d_ictHeader.ScanParameter.Azimuth = d_angle;
	d_ictHeader.ScanParameter.NumberofValidVerticalDetector = d_channelNum;
	//对无关参数设置默认值
	d_ictHeader.ScanParameter.RadialDistanceInLocal = 0;
	d_ictHeader.ScanParameter.AngleInLocal = 0;
	d_ictHeader.ScanParameter.HelixScanPitch = 0;
	d_ictHeader.ScanParameter.FirstSectStartCoordinateOfDR = 0;
	d_ictHeader.ScanParameter.SecondSectStartCoordinateOfDR = 0;
	d_ictHeader.ScanParameter.TotalLayers2 = 0;
	d_ictHeader.ScanParameter.Ct2ScanMode = 0;
	d_ictHeader.ScanParameter.DataTransferMode = 1;
	d_ictHeader.ScanParameter.NumberOfTranslation = 0;
	d_ictHeader.ReconstructParameter.NumberOfGraduationOfCt2 = 0;
	d_ictHeader.ReconstructParameter.SerialOfGraduationOfCt2 = 0;
	d_ictHeader.ScanParameter.ViewDiameter = d_viewDiameter;
	d_ictHeader.ScanParameter.NumberOfTable = 1;
	d_ictHeader.ScanParameter.LargeViewCenterOffset = 0;
	d_ictHeader.ScanParameter.Pixels = d_matrix;
	d_ictHeader.ScanParameter.GraduationDirection = P_DIR;
	d_ictHeader.ScanParameter.DelaminationMode = 0;
	CalculateView_ValidDetector(d_view);
	d_ictHeader.ScanParameter.InterpolationFlag = d_setupData->lineDetData[d_lineDetIndex].StandartInterpolationFlag;
	d_ictHeader.ScanParameter.NumberOfInterpolation = 
		(float)d_matrix / d_ictHeader.ScanParameter.NumberOfValidHorizontalDetector + 1;
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

	return true;
}

void CT3Scan::saveTempFile(LineDetList* in_listHead)
{
	auto listItem = in_listHead;
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
	d_tempFileVec.push_back(tempFileName);
}

void CT3Scan::stopScan()
{
	d_threadRun = false;
}
