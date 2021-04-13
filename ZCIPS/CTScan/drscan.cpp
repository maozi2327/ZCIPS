#include "stdafx.h"
#include "drscan.h"
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


DrScan::DrScan(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork, 
	const SetupData* _setupData, int _lineDetIndex, LineDetImageProcess* _lineDetImageProcess)
	: LineDetScanInterface(_controller, _lineDetNetWork, _setupData, _lineDetIndex, _lineDetImageProcess)
{
}


DrScan::~DrScan()
{
}

bool DrScan::caculateParemeterAndSetGenerialFileHeader()
{
	LineDetScanInterface::caculateParemeterAndSetGenerialFileHeader();

	d_ictHeader.ScanParameter.SampleTime = float(d_sampleTime) / 1000;
	d_ictHeader.ScanParameter.SetupSynchPulseNumber
		= (WORD)(d_ictHeader.ScanParameter.SampleTime * d_ictHeader.SystemParameter.SynchFrequency);

	d_ictHeader.ScanParameter.NumberOfGraduation = d_matrix;
	d_ictHeader.ScanParameter.Azimuth = d_angle;
	d_ictHeader.ScanParameter.NumberofValidVerticalDetector = d_channelNum;
	//对无关参数设置默认值
	d_ictHeader.ScanParameter.Pixels = d_matrix;
	d_ictHeader.ScanParameter.InterpolationFlag = d_setupData->lineDetData[d_lineDetIndex].StandartInterpolationFlag;
	d_ictHeader.ScanParameter.ScanMode = static_cast<char>(ScanMode::DR_SCAN);
	int N = d_ictHeader.ScanParameter.NumberOfSystemHorizontalDetector;
	float d = PI * d_ictHeader.ScanParameter.HorizontalSectorAngle / (180 * (N - 1));
	d *= d_ictHeader.ScanParameter.SourceDetectorDistance;
	N = d_ictHeader.ScanParameter.NumberOfInterpolation;

	if ((d / N < d_setupData->minInterpolationSpace) || (N > 80))
	{
		LOG_ERROR("插值次数太多！视场直径%.0f, 图像矩阵%d", d_view, d_matrix);
		return false;
	}
	
	d_ictHeader.ScanParameter.FirstSectStartCoordinateOfDR = d_layerStartPoint;
	d_ictHeader.ScanParameter.CurrentLayerCoordinate = d_layerStartPoint;

	if (std::abs(d_layerSpace - 0) < 0.001)
		d_ictHeader.ScanParameter.SpaceBetweenLayer = d_view / d_matrix + 0.5;
	else
		d_ictHeader.ScanParameter.SpaceBetweenLayer = d_layerSpace;
	
	d_ictHeader.ScanParameter.TotalLayers	= d_layerLenth / d_ictHeader.ScanParameter.SpaceBetweenLayer;

	if (d_ictHeader.ScanParameter.InterpolationFlag == 1)
		d_allScanTotalSamples = d_currentScanTotalSamples = (d_ictHeader.ScanParameter.NumberOfInterpolation + 1) * d_ictHeader.ScanParameter.TotalLayers;
	else 
		d_allScanTotalSamples = d_currentScanTotalSamples = d_ictHeader.ScanParameter.NumberOfInterpolation * d_ictHeader.ScanParameter.TotalLayers;

	return true;
}

bool DrScan::setScanParameter(float _layer, int _matrix, float _view, int _sampleTime, float _angle, float _layerLenth, float _layerSpace)
{
	d_layerStartPoint = _layer;
	d_matrix = _matrix;
	d_view = _view;
	d_sampleTime = _sampleTime;
	d_angle = _angle;
	d_layerLenth = _layerLenth;
	d_layerSpace = _layerSpace;
	return true;
}

void DrScan::sendCmdToControl()
{
	DRScanCmdData cmdData;
	cmdData.stsBit.s.physiInterpolation	= d_setupData->lineDetData[d_lineDetIndex].PhysicsInterpolationFlag;
	cmdData.stsBit.s.multiAngle = d_setupData->drScanData[d_lineDetIndex].drScanAngleDefine;
	cmdData.stsBit.s.twoVehicle = 0;
	cmdData.stsBit.s.czAmountInc1 = d_ictHeader.ScanParameter.InterpolationFlag;
	cmdData.stsBit.s.biDirScan = d_setupData->drScanData[d_lineDetIndex].drScanModeDefine;
	cmdData.stsBit.s.btnStartScan = 0;
	cmdData.stsBit.s.autoStopBeam = 0;
	cmdData.interpolationAmount	= d_ictHeader.ScanParameter.NumberOfInterpolation;
	cmdData.projectionAmount = d_ictHeader.ScanParameter.TotalLayers;
	cmdData.angleAmount = 1;
	cmdData.sampleTime	= 1000 * d_ictHeader.ScanParameter.SampleTime;
	cmdData.viewDiameter = d_ictHeader.ScanParameter.ViewDiameter;
	cmdData.orientInc = d_ictHeader.ScanParameter.Azimuth;
	cmdData.layerSpace = d_ictHeader.ScanParameter.SpaceBetweenLayer;
	cmdData.firstLayerOffset = d_ictHeader.ScanParameter.FirstSectStartCoordinateOfDR;
	cmdData.tabelNumber = 0;
	cmdData.reserved1 = 0;
	cmdData.reserved2 = 0;
	cmdData.centerOffset = 0;
	d_controller->sendToControl(CMD_DR_SCAN, (char*)(&cmdData), sizeof(CT23ScanCmdData), false);
}

void DrScan::saveFile()
{
	d_ictHeader.ScanParameter.Azimuth = d_controller->readAxisPostion(Axis::objRotation);
	QDir  dir;
	QString orgPath = d_orgName.left(d_orgName.lastIndexOf('/') + 1);

	if (!dir.exists(orgPath))
		dir.mkpath(orgPath);

	QString timeName = getTimeWithUnderline();
	d_orgName = orgPath + timeName + d_orgName.right(d_orgName.length() - d_orgName.lastIndexOf('_'));
	saveOrgFile();

	if (!dir.exists(d_filePath))
		dir.mkpath(d_filePath);

	d_lineDetImageProcess->drTune(d_orgName, d_filePath);

	if (!d_saveOrg)
		QFile::remove(d_orgName);
}

void DrScan::saveTempFile(LineDetList * _listHead)
{
}
