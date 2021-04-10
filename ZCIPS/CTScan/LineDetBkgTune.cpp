#include "stdafx.h"
#include "linedetbkgtune.h"
#include "controllerinterface.h"
#include "../Public/util/Thread.h"
#include "LineDetImageProcess.h"
#include "LineDetNetWork.h"

LineDetBkgTune::LineDetBkgTune(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork,
	const SetupData* _setupData, int _lineDetIndex, LineDetImageProcess* _lineDetImageProcess)
	: LineDetScanInterface(_controller, _lineDetNetWork, _setupData, _lineDetIndex, _lineDetImageProcess)
{
}


LineDetBkgTune::~LineDetBkgTune()
{
}

void LineDetBkgTune::sendCmdToControl()
{
	d_controller->sendToControl(CMD_BKG_TUNE, nullptr, 0, false);
}

bool LineDetBkgTune::caculateParemeterAndSetGenerialFileHeader()
{
	LineDetScanInterface::caculateParemeterAndSetGenerialFileHeader();

	d_ictHeader.ScanParameter.SampleTime = float(d_sampleTime) / 1000;
	d_ictHeader.ScanParameter.SetupSynchPulseNumber
		= (WORD)(d_ictHeader.ScanParameter.SampleTime * d_ictHeader.SystemParameter.SynchFrequency);
	d_ictHeader.ScanParameter.NumberOfGraduation = TUNE_PROJECTIONS;
	d_ictHeader.ScanParameter.Azimuth = 0;
	d_ictHeader.ScanParameter.NumberofValidVerticalDetector = d_channelNum;
	d_currentScanTotalSamples = d_ictHeader.ScanParameter.ViewDiameter = TUNE_PROJECTIONS;
	d_ictHeader.ScanParameter.Pixels = TUNE_PROJECTIONS;
	d_ictHeader.ScanParameter.InterpolationFlag = d_setupData->lineDetData[d_lineDetIndex].StandartInterpolationFlag;
	d_ictHeader.ScanParameter.NumberOfInterpolation = 1;
	d_ictHeader.ScanParameter.ScanMode = static_cast<unsigned short>(ScanMode::BKG_SCAN);
	return true;
}

void LineDetBkgTune::saveFile()
{
	saveOrgFile(d_lineDetNetWork->getRowList(), d_orgName);
	d_lineDetImageProcess->createBkgDat(d_orgName, d_filePath);
}

void LineDetBkgTune::saveTempFile(LineDetList * _listHead)
{

}
