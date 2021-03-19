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
	const SetupData* _setupData, int _lineDetIndex)
	: LineDetScanInterface(_controller, _lineDetNetWork, _setupData, _lineDetIndex)
{
	
}

CT3Scan::~CT3Scan()
{
	if(d_scanThread.get() != nullptr)
		d_scanThread->stopThread();
}

bool CT3Scan::setScanParameter(float _layer, int _matrix, float _view, 
	int _sampleTime, float _angle)
{
	d_layer = _layer;
	d_matrix = _matrix;
	d_view = _view;
	d_sampleTime = _sampleTime;
	d_angle = _angle;
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
//		setGenerialFileHeader();
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
//			emit(signalGraduationCount(100 * d_lineDetNetWork->getGraduationCount() / d_allGraduationSample));
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
	CT23ScanCmdData	cmdData, *pCmdData;
	cmdData.stsBit.s.changeLayerSpace = 0;
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
	cmdData.firstLayerOffset = d_layer;
	cmdData.layerSpace = 0;
	d_controller->sendToControl(CMD_CT_SCAN, (char*)(&cmdData), sizeof(CT23ScanCmdData), false);
}

void CT3Scan::saveFile()
{
	//TODO_DJ：断续扫描合成完整文件
	//std::unique_ptr<RowList> finalList(new RowList);

	//for (auto& tempFile : d_tempFileVec)
	//{
	//	QFile file;
	//	file.setFileName(tempFile);
	//	file.open(QIODevice::ReadWrite);
	//	auto byteArray = file.readAll();
	//	int listItemNum = byteArray.size() / d_lineDetNetWork->getListItemSize();
	//	auto listItemSize = d_lineDetNetWork->getListItemSize();
	//	char* memory = new char[byteArray.size()];
	//	memcpy(memory, byteArray.data(), byteArray.size());

	//	for (int i = 0; i != listItemNum; ++i)
	//		finalList->push_back((unsigned long*)(memory + listItemSize * i));
	//}	
	//
	//saveOrgFile(finalList.get()->getList());
	//TODO_DJ：断续扫描合成完整文件

	//从数据库中检索文件当天扫描的文件编号，如果有同名文件就当前最大编号+1，无同名文件就新建编号0000
	//auto number = d_fileDB->getNumber(d_fileName);

	QString orgDirectory;
	QDir  logDir;
	QString orgPath = d_orgName.left(d_orgName.lastIndexOf('/') + 1);

	if (!logDir.exists(orgPath))
		logDir.mkpath(d_orgName.left(d_orgName.lastIndexOf('/') + 1));

	if (!logDir.exists(d_filePath))
		logDir.mkpath(d_filePath);

	QDir dir(d_filePath);
	int filesNumber = 0;
	dir.setFilter(QDir::Dirs | QDir::Files);
	dir.setSorting(QDir::DirsFirst);
	int fileNumber = dir.count();
	d_orgName = orgPath + QString::number(fileNumber + 1) + d_orgName.right(d_orgName.length() - d_orgName.lastIndexOf('_'));
	saveOrgFile(d_lineDetNetWork->getRowList(), d_orgName);

	//TODO_DJ：保存ORG记录到数据库
	//OrgRecord orgRecord;
	//orgRecord.path = d_orgPath;
	//orgRecord.fileName = d_fileName;
	//orgRecord.number = number.toInt();
	//orgRecord.type = static_cast<char>(ScanMode::CT3_SCAN);
	//d_fileDB->writeOrgRecord(orgRecord);
	//TODO_DJ：保存ORG记录到数据库
	
	//拷贝空气文件到校正参数路径
	QFile::copy(d_airFile, d_installDirectory + "air.dat");
	d_lineDetImageProcess->ct3Dispose(d_orgName, d_filePath);

	//TODO_DJ：保存CT记录到数据库
	//CT3Record ct3Record;
	//ct3Record.path = d_filePath;
	//ct3Record.fileName = d_fileName;
	//ct3Record.number = orgRecord.number;
	//ct3Record.view = d_viewDiameter;
	//ct3Record.matrix = d_matrix;
	//ct3Record.sampleTime = d_sampleTime;
	//ct3Record.layer = d_layer;
	//TODO_DJ：保存CT记录到数据库
}

bool CT3Scan::setGenerialFileHeader()
{
	LineDetScanInterface::setGenerialFileHeader();

	d_ictHeader.ScanParameter.SampleTime = float(d_sampleTime) / 1000;
	d_ictHeader.ScanParameter.SetupSynchPulseNumber
		= (WORD)(d_ictHeader.ScanParameter.SampleTime * d_ictHeader.SystemParameter.SynchFrequency);

	d_ictHeader.ScanParameter.NumberOfGraduation = d_matrix;
	d_ictHeader.ScanParameter.Azimuth = d_angle;
	d_ictHeader.ScanParameter.NumberofValidVerticalDetector = d_channelNum;
	//对无关参数设置默认值
	d_ictHeader.ScanParameter.ViewDiameter = d_viewDiameter;
	d_ictHeader.ScanParameter.Pixels = d_matrix;
	CalculateView_ValidDetector(d_view);
	d_ictHeader.ScanParameter.InterpolationFlag = d_setupData->lineDetData[d_lineDetIndex].StandartInterpolationFlag;
	d_ictHeader.ScanParameter.NumberOfInterpolation =
		(float)d_matrix / d_ictHeader.ScanParameter.NumberOfValidHorizontalDetector + 1;

	d_ictHeader.ScanParameter.ScanMode = static_cast<char>(ScanMode::CT3_SCAN);
	CalculateView_ValidDetector(d_view);
	d_allGraduationSample = d_ictHeader.ScanParameter.NumberOfInterpolation * d_matrix;
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

	if (!QFile::exists(d_airFile))
		return false;

	return true;
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
