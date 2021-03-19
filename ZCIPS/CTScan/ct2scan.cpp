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
	const SetupData* _setupData, int _lineDetIndex)
	: LineDetScanInterface(_controller, _lineDetNetWor, _setupData, _lineDetIndex)
{
}

Ct2Scan::~Ct2Scan()
{
}

bool Ct2Scan::scanFinished()
{
	if (d_graduationCount == d_imageAmount)
		return true;

	return false;
}

void Ct2Scan::saveFile()
{
	++d_graduationCount;
	QString orgDirectory;

	if (d_graduationCount == 1)
	{
		QDir  logDir;
		QString orgPath = d_orgName.left(d_orgName.lastIndexOf('/') + 1);

		if (!logDir.exists(orgPath))
			logDir.mkpath(d_orgName.left(d_orgName.lastIndexOf('/') + 1));

		if (!logDir.exists(d_filePath))
			logDir.mkpath(d_filePath);
	}

	QString number;
	number.sprintf("%3d", d_graduationCount);
	QString orgName = d_orgName.left(d_orgName.lastIndexOf('.')) + number + QString::fromLocal8Bit(".org");
	saveOrgFile(d_lineDetNetWork->getRowList(), orgName);
	QFile::copy(d_airFile, d_installDirectory + "air.dat");
	d_lineDetImageProcess->ct3Dispose(orgName, d_orgName.left(d_orgName.lastIndexOf('/') + 1));

	if (d_graduationCount == d_imageAmount)
	{
		QString finalCt2FileName, pureFileName;
		pureFileName = d_orgName.left(d_orgName.right(d_orgName.length() - d_orgName.lastIndexOf('/') - 1).lastIndexOf('.'));
		finalCt2FileName = d_orgName.left(d_orgName.lastIndexOf('/') + 1) + pureFileName + QString::fromLocal8Bit("000.pcd");
		QString destFileName = d_filePath + pureFileName + QString::fromLocal8Bit(".pcd");
		QFile::copy(finalCt2FileName, destFileName);
	}
}

bool Ct2Scan::setScanParameter(float _layer, int _matrix, float _view, int _sampleTime, float _angle, int _imageAmount)
{
	d_layer = _layer;
	d_matrix = _matrix;
	d_view = _view;
	d_sampleTime = _sampleTime;
	d_angle = _angle;
	d_imageAmount = _imageAmount;
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
	cmdData.ct2Mode	= d_ictHeader.ScanParameter.Ct2ScanMode;
	cmdData.firstLayerOffset = d_layer;
	cmdData.layerSpace = 0;
	d_controller->sendToControl(CMD_CT_SCAN, (char*)(&cmdData), sizeof(CT23ScanCmdData), false);
}

bool Ct2Scan::setGenerialFileHeader()
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

	d_ictHeader.ScanParameter.ScanDirectionMode = d_setupData->ct2Data[d_lineDetIndex].translationModeDefine;
	d_ictHeader.ScanParameter.Ct2ScanMode = d_setupData->ct2Data[d_lineDetIndex].Ct2ScanMode;				//2代扫描模式:  0-360°,1-180°,2-180°间隔
	return true;
}











