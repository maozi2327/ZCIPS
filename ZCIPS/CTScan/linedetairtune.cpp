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
	d_ictHeader.ScanParameter.NumberOfTable = 1;					//У��ɨ��ת̨����
	d_ictHeader.ScanParameter.LargeViewCenterOffset = 0;			//���ӳ�ɨ���ת����ƫ��(ͬʱ���ڶ�ת̨ɨ���ת����ƫ��)
	d_ictHeader.ScanParameter.Pixels = TUNE_PROJECTIONS;			//V2.3�ж���Ϊ����ƽ��ͼ�����ظ���(ͶӰ��M), ȡ��V2.2�д��ű�ʾ��ʽ

	d_ictHeader.ScanParameter.Azimuth = 0;							//CTɨ��ʱ�ֶ���ʼ��λ��(��),��DRɨ��Ƕ�
	d_ictHeader.ScanParameter.SpaceBetweenLayer						//CT���/DR�ֲ���, ��λ:mm
		=0;
	d_ictHeader.ScanParameter.GraduationDirection = P_DIR;			//�ֶȷ���:N_DIR-˳ʱ��(������), P_DIR-��ʱ��(������)

	d_ictHeader.ScanParameter.ViewDiameter = 300;
	d_ictHeader.ScanParameter.NumberOfValidHorizontalDetector = 
		d_setupData->lineDetData[d_lineDetIndex].NumberOfSystemHorizontalDetector;
	d_ictHeader.ScanParameter.InterpolationFlag = 0;
	d_ictHeader.ScanParameter.NumberOfInterpolation = 1;
	d_allGraduationSample = d_ictHeader.ScanParameter.NumberOfGraduation = TUNE_PROJECTIONS;
	d_ictHeader.ScanParameter.DelaminationMode = 0;
	d_ictHeader.ScanParameter.TotalLayers = 1;
	return false;
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
	saveOrgFile(d_lineDetNetWork->getRowList());
	d_lineDetImageProcess->createAirDat(d_fileName, d_installDirectory);
}



bool LineDetAirTune::checkScanAble()
{
	return false;
}

bool LineDetAirTune::canScan()
{
	return false;
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
