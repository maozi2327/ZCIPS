#include "stdafx.h"
#include "LineDetNetWork.h"
#include "LineDetAirTune.h"
#include "LineDetImageProcess.h"
#include "../Public/util/Thread.h"
#include "../Public/util/functions.h"
#include "controllerinterface.h"

LineDetAirTune::LineDetAirTune(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork,
	const SetupData* _setupData, int _lineDetIndex)
	: LineDetScanInterface(_controller, _lineDetNetWork, _setupData, _lineDetIndex)
{

}

LineDetAirTune::~LineDetAirTune()
{

}

bool LineDetAirTune::setGenerialFileHeader()
{
	LineDetScanInterface::setGenerialFileHeader();

	d_ictHeader.ScanParameter.SampleTime = float(d_sampleTime) / 1000;
	d_ictHeader.ScanParameter.SetupSynchPulseNumber
		= (WORD)(d_ictHeader.ScanParameter.SampleTime * d_ictHeader.SystemParameter.SynchFrequency);
	d_ictHeader.ScanParameter.NumberOfGraduation = TUNE_PROJECTIONS;
	d_ictHeader.ScanParameter.Azimuth = 0;
	d_ictHeader.ScanParameter.NumberofValidVerticalDetector = d_channelNum;
	d_allGraduationSample = d_ictHeader.ScanParameter.ViewDiameter = TUNE_PROJECTIONS;
	d_ictHeader.ScanParameter.Pixels = TUNE_PROJECTIONS;
	d_ictHeader.ScanParameter.InterpolationFlag = d_setupData->lineDetData[d_lineDetIndex].StandartInterpolationFlag;
	d_ictHeader.ScanParameter.NumberOfInterpolation = 1;
	d_ictHeader.ScanParameter.ScanMode = static_cast<unsigned short>(ScanMode::AIR_SCAN);
	return true;
}

#define	RTBUF_LEN	256						//定义接收/发送缓冲区长度
//struct COMM_PACKET {
//	BYTE	tagHead[3];						//包头(0x55,0xaa,0x5a)
//	BYTE	typeCode;						//类型码
//	WORD	tagLen;							//包长(=参数字节数+3, 实际命令数据包长度=包长+4)
//	BYTE	data[RTBUF_LEN - 6];			//命令参数
//};

void LineDetAirTune::sendCmdToControl()
{
	char buf[RTBUF_LEN];
	COMM_PACKET* ptr = (COMM_PACKET*)buf;
	ptr->typeCode = static_cast<char>(ScanMode::AIR_SCAN);
	d_controller->sendToControl(static_cast<char>(ScanMode::AIR_SCAN), nullptr, 0, false);
}

void LineDetAirTune::scanThread()
{
	if (d_lineDetNetWork->startExtTrigAcquire())
	{
		static std::chrono::steady_clock::time_point last_time;
		last_time = d_start_time = std::chrono::steady_clock::now();
		setGenerialFileHeader();
		sendCmdToControl();
		d_lineDetNetWork->clearRowList();
		std::this_thread::sleep_for(std::chrono::seconds(3));

		while (d_deadThreadRun)
		{
			emit(signalGraduationCount(100 * d_lineDetNetWork->getGraduationCount() / d_allGraduationSample));

			if (d_controller->readSaveStatus())
			{
				saveFile();
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	else
		;
}

void LineDetAirTune::saveFile()
{
	saveOrgFile(d_lineDetNetWork->getRowList(), d_orgName);
	d_lineDetImageProcess->createAirDat(d_orgName, d_filePath);
}

bool LineDetAirTune::checkScanAble()
{
	return false;
}

bool LineDetAirTune::canScan()
{
	return false;
}

void LineDetAirTune::saveTempFile(LineDetList * _listHead)
{
}

void LineDetAirTune::stopScan()
{
	LineDetScanInterface::stopScan();
}

bool LineDetAirTune::beginScan()
{
	if (!checkScanAble())
		return false;

	setGenerialFileHeader();
	sendCmdToControl();
	d_scanThread.reset(new Thread(std::bind(&LineDetAirTune::scanThread, this), std::ref(d_deadThreadRun)));
	d_scanThread->detach();
	return true;
}
