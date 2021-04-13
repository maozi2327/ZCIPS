#include "stdafx.h"
#include "ct2scan.h"
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


Ct2Scan::Ct2Scan(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWor, 
	const SetupData* _setupData, int _lineDetIndex, LineDetImageProcess* _lineDetImageProcess)
	: LineDetScanInterface(_controller, _lineDetNetWor, _setupData, _lineDetIndex, _lineDetImageProcess)
{
}

Ct2Scan::~Ct2Scan()
{
}

bool Ct2Scan::scanFinished()
{
	if (d_graduationCount == d_ictHeader.ScanParameter.NumberOfGraduation)
		return true;

	return false;
}

//ORG文件名仍然为..文件名.org
//org和合并前的PCD都保存到org文件夹中
void Ct2Scan::saveFile()
{
	++d_graduationCount;
	QString orgPath = d_orgName.left(d_orgName.lastIndexOf('/') + 1);

	if (d_graduationCount == 1)
	{
		QDir  logDir;

		if (!logDir.exists(orgPath))
			logDir.mkpath(d_orgName.left(d_orgName.lastIndexOf('/') + 1));

		if (!logDir.exists(d_filePath))
			logDir.mkpath(d_filePath);
	}

	d_ictHeader.ReconstructParameter.SerialOfGraduationOfCt2 = d_graduationCount;
	QString number;
	number.sprintf("%04d", d_graduationCount);
	QString orgName = d_orgName.left(d_orgName.lastIndexOf('.')) + number + QString::fromLocal8Bit(".org");
	saveOrgFile(d_lineDetNetWork->getRowList(), orgName);
	d_lineDetNetWork->clearRowList();
	d_lineDetImageProcess->ct3Tune(orgName, d_orgName.left(d_orgName.lastIndexOf('/') + 1));

	if (d_graduationCount == d_ictHeader.ScanParameter.NumberOfGraduation)
	{
		QString finalCt2FileName, pureFileName;
		pureFileName = getPureFileNameFromFullName(d_orgName);
		finalCt2FileName = d_orgName.left(d_orgName.lastIndexOf('/') + 1) + pureFileName + QString::fromLocal8Bit("0000.pcd");
		QString timeName = getTimeWithUnderline();
		QString tunedFileName = d_filePath + timeName + pureFileName.right(pureFileName.length() - pureFileName.lastIndexOf('_')) + 
			QString::fromLocal8Bit(".pcd");
		QFile::copy(finalCt2FileName, tunedFileName);
	}
}

//TODO_DJ：修改其他线阵扫描文件名，保存时间
//TODO_DJ：等待控制系统连接后再初始化探测器
bool Ct2Scan::setScanParameter(float _layer, int _matrix, float _view, int _sampleTime, float _angle, int _ct2Mode)
{
	d_layer = _layer;
	d_matrix = _matrix;
	d_view = _view;
	d_sampleTime = _sampleTime;
	d_angle = _angle;
	d_ct2Mode = _ct2Mode;
	return true;
}

void Ct2Scan::sendCmdToControl()
{
	CT23ScanCmdData	cmdData, *pCmdData;
	cmdData.stsBit.s.changeLayerSpace = 0;
	cmdData.stsBit.s.czAmountInc1 = d_ictHeader.ScanParameter.InterpolationFlag
		= d_setupData->lineDetData[d_lineDetIndex].StandartInterpolationFlag;
	cmdData.stsBit.s.currentLayer = 1;
	cmdData.stsBit.s1.physiInterpolation = d_setupData->lineDetData[d_lineDetIndex].PhysicsInterpolationFlag;
	cmdData.stsBit.s.thirdGeneration = 0;
	cmdData.stsBit.s.btnStartScan = 0;
	cmdData.stsBit.s.autoStopBeam = 0;
	cmdData.stsBit.s.biDirTranslation = d_ictHeader.ScanParameter.ScanDirectionMode;

	cmdData.interpolationAmount = (BYTE)d_ictHeader.ScanParameter.NumberOfInterpolation;
	cmdData.projectionAmount = d_ictHeader.ScanParameter.Pixels;
	d_ictHeader.ScanParameter.SampleTime = float(d_sampleTime / 1000);
	cmdData.sampleTime = d_sampleTime;
	cmdData.viewDiameter = d_ictHeader.ScanParameter.ViewDiameter;
	cmdData.orientInc = (short)d_ictHeader.ScanParameter.Azimuth;
	cmdData.sliceAmount = d_ictHeader.ScanParameter.TotalLayers;
	cmdData.tabelNumber = 0;
	cmdData.reserved1 = 0;
	cmdData.centerOffset = 0;
	cmdData.ct2Mode	= d_ictHeader.ScanParameter.Ct2ScanMode;
	cmdData.firstLayerOffset = d_layer;
	cmdData.layerSpace = 0;
	d_controller->sendToControl(CMD_CT_SCAN, (char*)(&cmdData), sizeof(CT23ScanCmdData), false);
}

bool Ct2Scan::caculateParemeterAndSetGenerialFileHeader()
{
	LineDetScanInterface::caculateParemeterAndSetGenerialFileHeader();

	d_ictHeader.ScanParameter.LeftSerialNoOfTable = 0;				//左侧小转台序号(0开始)
	d_ictHeader.ScanParameter.NumberOfTable = 1;						//2代扫描转台数量
	d_ictHeader.ScanParameter.LargeViewCenterOffset = 0;				//大视场扫描回转中心偏置

	d_ictHeader.ScanParameter.ScanDirectionMode	= d_setupData->ct2Data[0].translationModeDefine;
	d_ictHeader.ScanParameter.GraduationDirection = P_DIR;
	d_ictHeader.ScanParameter.Azimuth = d_angle;
	d_ictHeader.ScanParameter.Ct2ScanMode = 0;				//2代扫描模式:  0-360°,1-180°,2-180°间隔
	d_ictHeader.ScanParameter.DataTransferMode	= 1;
	d_ictHeader.ScanParameter.TotalLayers = 1;

	d_ictHeader.ScanParameter.NumberOfValidHorizontalDetector =
		d_setupData->lineDetData[0].Ct2GraduationAngle / d_setupData->lineDetData[0].HorizontalSectorAngle
		* d_setupData->lineDetData[0].NumberOfSystemHorizontalDetector;
	float cta2Graduation = d_setupData->lineDetData[0].Ct2GraduationAngle * PI / 180;			//2代扫描分度角2012.02.19
	d_ictHeader.ScanParameter.SpaceBetweenLayer = d_layer;

	//确定像素间距
	d_ictHeader.ScanParameter.Pixels = d_matrix;
	d_ictHeader.ScanParameter.RadialPosition = 980;

	////二代内切方式------------------
	//确定平移距离
	//B = 2*ictHeader.ScanParameter.RadialPosition*(float)tan(cta2Sector/2);
	//S = D;
	//t = S /M;						//像素间距
	//S = D/ (2 * cos(cta2Sector/2)) + B / 2;
	//ictHeader.ScanParameter.NumberOfTranslation = 2 * S / t + 0.5;  
	//ictHeader.ScanParameter.ViewDiameter = ictHeader.ScanParameter.NumberOfTranslation * t;//实际像素间距
	//二代内切方式------------------

	//////二代原始中心切方式------------------
	float B = 2 * d_ictHeader.ScanParameter.RadialPosition*(float)tan(cta2Graduation / 2);
	//确定平移距离
	float S = std::max(d_view, B);
	float t = S / d_matrix;
	d_ictHeader.ScanParameter.NumberOfTranslation = d_matrix;
	d_view = d_ictHeader.ScanParameter.ViewDiameter = S;
	//////二代原始中心切方式------------------

	//确定2代/局部扫描分度数
	if (d_ictHeader.ScanParameter.Ct2ScanMode == _CT2_360)
		d_ictHeader.ScanParameter.NumberOfGraduation = (WORD)(2 * PI / cta2Graduation + 0.5);
	else
		d_ictHeader.ScanParameter.NumberOfGraduation = ((WORD)(2 * PI / cta2Graduation + 0.5)) / 2;

	d_ictHeader.ScanParameter.DelaminationMode = 0;						//CT分层方式:0-当前层, 1-等层距, 2-不等层距
	d_ictHeader.ReconstructParameter.NumberOfGraduationOfCt2 = (BYTE)d_ictHeader.ScanParameter.NumberOfGraduation;
	d_ictHeader.ScanParameter.SampleTime = float(d_sampleTime) / 1000;
	d_ictHeader.ScanParameter.SetupSynchPulseNumber
		= (WORD)(d_ictHeader.ScanParameter.SampleTime * d_ictHeader.SystemParameter.SynchFrequency);

	d_ictHeader.ScanParameter.Azimuth = d_angle;
	d_ictHeader.ScanParameter.NumberofValidVerticalDetector = d_channelNum;
	d_ictHeader.ScanParameter.ScanMode = static_cast<char>(ScanMode::CT2_SCAN);
	d_currentScanTotalSamples = d_ictHeader.ScanParameter.NumberOfTranslation;
	d_allScanTotalSamples = d_ictHeader.ScanParameter.NumberOfTranslation * d_ictHeader.ScanParameter.NumberOfGraduation;
	d_ictHeader.ScanParameter.ScanDirectionMode = d_setupData->ct2Data[d_lineDetIndex].translationModeDefine;
	return true;
}

void Ct2Scan::saveTempFile(LineDetList * _listHead)
{
}

bool Ct2Scan::beginScan()
{
	d_graduationCount = 0;
	return LineDetScanInterface::beginScan();
}

void Ct2Scan::setFileName(const QString & _orgName, const QString & _destPath)
{
	QString timeName = getTimeWithUnderline();
	d_orgName = _orgName.left(_orgName.lastIndexOf('/') + 1) + timeName +
		_orgName.right(_orgName.length() - _orgName.lastIndexOf('_'));
	d_filePath = _destPath;
}











