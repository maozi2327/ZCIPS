#include "stdafx.h"
#include "LineDetNetWork.h"
#include "LineDetAirTune.h"
#include "LineDetImageProcess.h"
#include "../Public/util/Thread.h"
#include "../Public/util/functions.h"
#include "controllerinterface.h"

LineDetAirTune::LineDetAirTune(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork,
	const SetupData* _setupData, int _lineDetIndex, LineDetImageProcess* _lineDetImageProcess)
	: LineDetScanInterface(_controller, _lineDetNetWork, _setupData, _lineDetIndex, _lineDetImageProcess)
{

}

LineDetAirTune::~LineDetAirTune()
{

}

bool LineDetAirTune::caculateParemeterAndSetGenerialFileHeader()
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
	d_ictHeader.ScanParameter.ScanMode = static_cast<unsigned short>(ScanMode::AIR_SCAN);
	return true;
}

#define	RTBUF_LEN	256						//�������/���ͻ���������
//struct COMM_PACKET {
//	BYTE	tagHead[3];						//��ͷ(0x55,0xaa,0x5a)
//	BYTE	typeCode;						//������
//	WORD	tagLen;							//����(=�����ֽ���+3, ʵ���������ݰ�����=����+4)
//	BYTE	data[RTBUF_LEN - 6];			//�������
//};

void LineDetAirTune::sendCmdToControl()
{
	char buf[RTBUF_LEN];
	COMM_PACKET* ptr = (COMM_PACKET*)buf;
	ptr->typeCode = CMD_RAY_TUNE;
	d_controller->sendToControl(CMD_RAY_TUNE, nullptr, 0, false);
}

void LineDetAirTune::saveTempFile(LineDetList * _listHead)
{
}

void LineDetAirTune::stopScan()
{
	LineDetScanInterface::stopScan();
}

void LineDetAirTune::saveFile()
{
	saveOrgFile();
	QDir dir;

	if (!dir.exists(d_filePath))
		dir.mkpath(d_filePath);

	d_lineDetImageProcess->createAirDat(d_orgName, d_filePath);

	if (!d_saveOrg)
		QFile::remove(d_orgName);
}
