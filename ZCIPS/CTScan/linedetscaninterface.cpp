#include "stdafx.h"
#include "linedetscaninterface.h"
#include "../Public/util/iulog.h"
#include "linedetnetwork.h"
#include "linedetimageprocess.h"
#include "../Public/util/thread.h"
#include "controllerinterface.h"
#include "filedb.h"
#include <algorithm>

ICT_HEADER LineDetScanInterface::d_ictHeader;
std::chrono::minutes LineDetScanInterface::d_intervalForSaveTempFile = std::chrono::minutes(30);

LineDetScanInterface::LineDetScanInterface(ControllerInterface * _controller, LineDetNetWork* _lineDetNetWork,
	const SetupData* _setupData, int _lineDetIndex, LineDetImageProcess* _lineDetImageProcess)
	: d_controller(_controller), d_lineDetNetWork(_lineDetNetWork), d_setupData(_setupData), d_scanThread(nullptr), d_saveOrg(true)
	, d_lineDetIndex(_lineDetIndex), d_lineDetImageProcess(_lineDetImageProcess)
{

}

LineDetScanInterface::~LineDetScanInterface()
{

}

void LineDetScanInterface::setFileName(const QString& _orgName, const QString& _destPath)
{
	d_orgName = _orgName;
	d_filePath = _destPath;
}

bool LineDetScanInterface::beginScan()
{
	if (!canScan())
		return false;

	if (d_scanThread)
		d_scanThread->stopThread();

	caculateParemeterAndSetGenerialFileHeader();
	sendCmdToControl();
	d_imageScaned = 0;
	d_samplesBefore = 0;
	d_lineDetNetWork->clearRowList();
	d_scanThread.reset(new Thread(std::bind(&LineDetScanInterface::scanThread, this), std::ref(d_deadThreadRun)));
	d_scanThread->detach();
	return true;
}

void LineDetScanInterface::stopScan()
{
	d_controller->stopAll();
	d_lineDetNetWork->stopAcquire(true);
	d_scanThread->stopThread();
}

//TODO_DJ：清除save标志
void LineDetScanInterface::scanThread()
{
	if (d_lineDetNetWork->startExtTrigAcquire())
	{
		static std::chrono::steady_clock::time_point last_time;
		last_time = d_start_time = std::chrono::steady_clock::now();
		std::this_thread::sleep_for(std::chrono::seconds(3));

		while (d_deadThreadRun)
		{
			auto now = std::chrono::steady_clock::now();

			if (now > last_time + d_intervalForSaveTempFile)
			{
				auto listTempHead = d_lineDetNetWork->getRowList();
				saveTempFile(listTempHead);
				d_lineDetNetWork->clearRowList();
				last_time = now;
			}

			int graduationCount = d_lineDetNetWork->getGraduationCount();
			int timeRemain = (d_allScanTotalSamples - (d_samplesBefore + graduationCount)) * d_sampleTime / 1000 / 60 + 1;
			QString message = makeFormatQString("剩余时间：%d分", timeRemain);
			emit scanProgressSignal(graduationCount, d_currentScanTotalSamples, d_samplesBefore + graduationCount,  d_allScanTotalSamples, message);

			if (d_controller->readSaveStatus())
			{
				saveFile();
				d_controller->clearSaveFlag();
				d_samplesBefore = d_samplesBefore + graduationCount;

				if (scanFinished())
				{
					emit scanThreadQuitSignal(0);
					return;
				}
				else
				{
					d_lineDetNetWork->clearRowList();
					std::this_thread::sleep_for(std::chrono::seconds(3));
				}
			}
			else if (d_controller->readWaitNextScanStatus())
			{
				d_controller->startNextScan();
				std::this_thread::sleep_for(std::chrono::seconds(3));
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		emit scanThreadQuitSignal(1);
	}
	else
		emit scanThreadQuitSignal(2);
}

void LineDetScanInterface::saveOrgFile(LineDetList* _List, const QString& _fileName)
{
	d_ictHeader.DataFormat.TotalLines = d_lineDetNetWork->getListItemNum();
	d_lineDetImageProcess->saveOrgFile(_fileName, &d_ictHeader, _List, 1);
}

bool LineDetScanInterface::scanFinished()
{
	return true;
}

bool LineDetScanInterface::caculateParemeterAndSetGenerialFileHeader()
{
	d_ictHeader.MainVersion = MainVersion23;
	d_ictHeader.SubVersion = SubVersion23;
	d_ictHeader.DataFormat.DataType = LONGDATA;
	d_ictHeader.DataFormat.appendColAtRow = 2;
	strcpy_s(d_ictHeader.SystemParameter.ModelOfCT, d_setupData->szDeviceModel);//CT设备型号
	d_ictHeader.DataFormat.dataColAtRow
		= d_setupData->lineDetData[d_lineDetIndex].NumberOfSystemHorizontalDetector;

	//TODO_DJ:
	//if (d_rayType == RayType::Accelerator)
	//{
	//	d_ictHeader.SystemParameter.RaySort = 0;
	//	d_ictHeader.SystemParameter.RayEngery = d_setupData->acceleratorData[d_AccIndex].rayEnergy;
	//	d_ictHeader.SystemParameter.RayDosage = d_setupData->acceleratorData[d_AccIndex].rayDoseRate;
	//	d_ictHeader.SystemParameter.SynchFrequency = d_setupData->acceleratorData[d_AccIndex].syncFreqDefine[d_accFrecIndex];
	//}
	//else if (d_rayType == RayType::Tube)
	//{
	//	d_ictHeader.SystemParameter.RaySort = 0;
	//	d_ictHeader.SystemParameter.RayEngery = d_setupData->acceleratorData[d_AccIndex].rayEnergy;
	//	d_ictHeader.SystemParameter.RayDosage = d_setupData->acceleratorData[d_AccIndex].rayDoseRate;
	//	d_ictHeader.SystemParameter.SynchFrequency = 250;
	//}

	//设置探测器参数
	d_ictHeader.SystemParameter.AmplifyMultiple = d_setupData->lineDetData[d_lineDetIndex].AmplifyMultiple;
	d_ictHeader.SystemParameter.IntegralTime = d_setupData->lineDetData[d_lineDetIndex].IntegralTime;
	d_ictHeader.ScanParameter.DetectorStyle
		= d_setupData->lineDetData[d_lineDetIndex].LineDetType;
	d_ictHeader.ScanParameter.NumberOfSystemHorizontalDetector
		= d_setupData->lineDetData[d_lineDetIndex].NumberOfSystemHorizontalDetector;
	d_ictHeader.ScanParameter.SerialNo1OfMiddleHorizontalDetector
		= d_setupData->lineDetData[d_lineDetIndex].SerialNo1OfMiddleHorizontalDetector;
	d_ictHeader.ScanParameter.SerialNo2OfMiddleHorizontalDetector
		= d_setupData->lineDetData[d_lineDetIndex].SerialNo2OfMiddleHorizontalDetector;
	d_ictHeader.ScanParameter.BeginSerialNoOfCt2Detector
		= d_setupData->lineDetData[d_lineDetIndex].BeginSerialNoOfCt2Detector;
	d_ictHeader.ScanParameter.EndSerialNoOfCt2Detector
		= d_setupData->lineDetData[d_lineDetIndex].EndSerialNoOfCt2Detector;
	d_ictHeader.ScanParameter.NumberofSystemVerticalDetector = 1;
	d_ictHeader.ScanParameter.SpaceOfHorizontalDetector	= 0;
	d_ictHeader.ScanParameter.SpaceOfVerticalDetector = 0;
	d_ictHeader.ScanParameter.HorizontalSectorAngle = d_setupData->lineDetData[d_lineDetIndex].HorizontalDetectorAngle;
	d_ictHeader.ScanParameter.VerticalSectorAngle = 0;
	d_ictHeader.ScanParameter.RadialPosition = d_controller->readAxisPostion(Axis::objRadial);
	d_ictHeader.ScanParameter.SourceDetectorDistance = d_controller->readAxisPostion(Axis::detRadial);
	d_ictHeader.ScanParameter.CollimationSize = d_colimateSize;
	d_ictHeader.ScanParameter.LayerThickness = 2/*d_layerThickness*/; //TODO_DJ
	d_ictHeader.ScanParameter.SampleTime = float(d_sampleTime) / 1000;
	d_ictHeader.ScanParameter.SetupSynchPulseNumber
		= (WORD)(d_ictHeader.ScanParameter.SampleTime * d_ictHeader.SystemParameter.SynchFrequency);
	
	d_ictHeader.DataFormat.graduationBase = 0;						//3代扫描起始分度
	d_ictHeader.ScanParameter.NumberOfGraduation = d_matrix;
	d_ictHeader.ScanParameter.Azimuth = d_angle;
	d_ictHeader.ScanParameter.NumberofValidVerticalDetector = d_channelNum;
	//对无关参数设置默认值
	d_ictHeader.ScanParameter.RadialDistanceInLocal = 0;
	d_ictHeader.ScanParameter.AngleInLocal = 0;
	d_ictHeader.ScanParameter.HelixScanPitch = 0;
	d_ictHeader.ScanParameter.FirstSectStartCoordinateOfDR = 0;
	d_ictHeader.ScanParameter.SecondSectStartCoordinateOfDR = 0;
	d_ictHeader.ScanParameter.TotalLayers = 1;
	d_ictHeader.ScanParameter.TotalLayers2 = 0;
	d_ictHeader.ScanParameter.Ct2ScanMode = 0;
	d_ictHeader.ScanParameter.DataTransferMode = 1;
	d_ictHeader.ScanParameter.NumberOfTranslation = 0;
	d_ictHeader.ReconstructParameter.NumberOfGraduationOfCt2 = 0;
	d_ictHeader.ReconstructParameter.SerialOfGraduationOfCt2 = 0;
	d_ictHeader.ScanParameter.NumberOfTable = 1;
	d_ictHeader.ScanParameter.LargeViewCenterOffset = 0;
	d_ictHeader.ScanParameter.Pixels = d_matrix;
	d_ictHeader.ScanParameter.GraduationDirection = P_DIR;
	d_ictHeader.ScanParameter.DelaminationMode = 0;
	
	d_ictHeader.ScanParameter.NumberOfValidHorizontalDetector = 
		CalculateView_ValidDetector(d_view, d_ictHeader.ScanParameter.ViewDiameter);
	
	d_ictHeader.ScanParameter.InterpolationFlag = d_setupData->lineDetData[d_lineDetIndex].StandartInterpolationFlag;
	d_ictHeader.ScanParameter.NumberOfInterpolation =
		(float)d_matrix / d_ictHeader.ScanParameter.NumberOfValidHorizontalDetector + 1;

	//设置时间
	QString time = QDateTime::currentDateTime().time().toString();
	QByteArray byteArray = time.toLatin1();
	strcpy_s(d_ictHeader.Task.DateTime.Time, byteArray.data());
	QString date = QDateTime::currentDateTime().date().toString(Qt::ISODate);
	byteArray = date.toLatin1();
	strcpy_s(d_ictHeader.Task.DateTime.Date, byteArray);

	strcpy_s(d_ictHeader.ScanParameter.FilenameTemperature, "");
	strcpy_s(d_ictHeader.ScanParameter.FilenameOfCTdata, "");
	strcpy_s(d_ictHeader.ScanParameter.FilenameOf2CTdata, "");
	strcpy_s(d_ictHeader.ScanParameter.FilenameOfPictureData, "");
	return true;
}

int LineDetScanInterface::CalculateView_ValidDetector(float _diameter, float& _viewDatmeter)
{
	int systemDetector = d_setupData->lineDetData[d_lineDetIndex].NumberOfSystemHorizontalDetector;
	int calibrateDetector = d_setupData->lineDetData[d_lineDetIndex].NumberOfCalibrateHorizontalDetector;
	int realSysDetectorNum = systemDetector - calibrateDetector;
	float delta = d_setupData->lineDetData[d_lineDetIndex].HorizontalDetectorAngle / 180 * PI;

	int leftMiddle = d_ictHeader.ScanParameter.SerialNo1OfMiddleHorizontalDetector;
	int rightMiddle = d_ictHeader.ScanParameter.SerialNo2OfMiddleHorizontalDetector;
	int Nv;

	if (false)		//确定3代扫描有效探测器数Nv
	{
		if (leftMiddle == rightMiddle)
			Nv = 2 * std::min<int>(rightMiddle, systemDetector - rightMiddle - 1) + 1;
		else
			Nv = 2 * std::min<int>(rightMiddle, systemDetector - rightMiddle);
		
		d_ictHeader.ScanParameter.NumberOfValidHorizontalDetector = Nv;
		_diameter = 2 * 980 * (float)sin(delta*(Nv - 1) / 2);
	}
	else 
	{
		float beta = (float)(2 * asin(_diameter / 2 / 980));        //计算视场D占用的扇角beta

		if (leftMiddle == rightMiddle)
			Nv = 2 * (int)(beta / 2 / delta) + 1;
		else															//探测器不通过扇面中心线，有效探测器设置为偶数
			Nv = 2 * (int)(beta / 2 / delta);

		if (Nv > realSysDetectorNum)
			Nv = realSysDetectorNum;

		d_ictHeader.ScanParameter.NumberOfValidHorizontalDetector = Nv;
		_diameter = 2 * 980 * (float)sin(delta*(Nv - 1) / 2);
	}
	
	_viewDatmeter = (float)((int)(100.0 * _diameter)) / 100;
	return Nv;
}

bool LineDetScanInterface::canScan()
{	
	if (!d_lineDetNetWork->getConnected())
	{
		LOG_ERROR("探测器未连接！");
		return false;
	}

	if (!d_controller->getConnected())
		return false;

	if (!d_controller->readReadyStatus())
		return false;

	if (!d_controller->readIdleStatus())
		return false;

	return true;
}

//加载零时文件读取已经采集的分度束
int LineDetScanInterface::loadTempFile()
{
	return 0;
}

void LineDetScanInterface::saveOrgFile()
{
	saveOrgFile(d_lineDetNetWork->getRowList(), d_orgName);
}
