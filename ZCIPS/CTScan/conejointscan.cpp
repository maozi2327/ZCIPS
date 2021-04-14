#include "stdafx.h"
#include "conejointscan.h"
#include "controllerinterface.h"
#include "imageprocess.h"
#include "../../Public/util/functions.h"
#include "../PanelDetector/panel.h"


//׶��ƴ��ɨ��ֻ�������ӳ���С����ɨ������ṩ������λ�ý���ɨ���ԭ���ǣ�ƴ��ɨ����Ҫһ�����غ�����������λ�ý���
//ɨ�裬����������Ա�ж��ӳ��ܷ���¹����޷�ȷ���ص�����
ConeJointScan::ConeJointScan(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _ctTune, bool _bkgTuneFlag,
	bool _airTuneFlag, bool _defectTuneFlag, PanDetData _pandetData, const QString& _airFileNameA, const QString& _airFileNameB)
	: ConeScanInterface(_panel, _controller, _ctTune, _bkgTuneFlag, _airTuneFlag, _defectTuneFlag, _pandetData)
	, d_sampleCount(0), d_airFileNameA(_airFileNameA), d_airFileNameB(_airFileNameB)
{

}


ConeJointScan::~ConeJointScan()
{
}


void ConeJointScan::scanThread()
{
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::function<void(unsigned short *)> frameCallback = std::bind(
		&ConeJointScan::frameProcessCallback, this, std::placeholders::_1);
	std::thread spliceThread;

	while (d_scanThreadRun)
	{
		printf("%d\n", d_scanProc);

		switch (d_scanProc) 
		{
		case 0:
			//��ʱ1s����Ϊ����һ������onstartnextscan��ctrlSysSts.s.waitNextScan���ܻ�û�и���
			//������waitNextScanΪtrue�ȴ���λ�����Ͷ�λ���¸�ɨ��λ�ý׶Σ������ǵȴ�������ת�׶Σ������ʱ�ȴ���λ�����º����
			//�����λ����λ��ɨ��λ�ã�waitNextScanΪtrue,��λ����ʱ2s��ʼ��תɨ��ɼ���
			if (d_controller->readReadyStatus()) 
			{
				if (d_scanTime == 0)
					d_imageProcess->loadAirData(d_airFileNameA);
				else if(d_scanTime == 1)
					d_imageProcess->loadAirData(d_airFileNameB);

				d_panel->beginExTriggerAcquire(frameCallback, d_cycleTime, 2);
				d_scanProc = 8;
			}
			break;
		case 8:
			d_controller->startNextScan();
			std::this_thread::sleep_for(std::chrono::seconds(2));
			d_scanProc = 12;
			break;
		case 12:
			if (d_controller->readSaveStatus()) 
				d_scanProc = 14;

			break;
		case 14:
			if (d_graduationCount >= d_graduation)
			{
				if (++d_sampleCount == d_round)
					d_scanProc = 16;
				else
					d_scanProc = 0;
			}
			break;
		case 16:
			std::this_thread::sleep_for(std::chrono::seconds(2));
			d_panel->stopAcquire();

			spliceThread = 	std::thread(
				std::bind(&PanelImageProcess::dataSplice, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
				d_imageProcess, d_pathA, d_pathB, d_pathDest, std::ref(d_spliceProgress));

			spliceThread.detach();
			d_scanProc = 20;
			break;
		case 20:
			QString spliceProgress = QString::number(d_spliceProgress);
			spliceProgress = QString::fromLocal8Bit("ƴ�ӽ���") + spliceProgress + QString::fromLocal8Bit("%");
			emit scanProgressSignal(d_graduationCount, d_graduation, d_graduationCount, d_round * d_graduation * d_framesPerGraduation, spliceProgress);
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void ConeJointScan::createFileName(QString& _orgFileName, QString& _tunedFileName)
{
	if (d_graduationCount % d_graduation == 0)
	{
		QString orgPath = d_orgName.left(d_orgName.lastIndexOf('/') + 1);
		QString timeName = getTimeWithUnderline();
		d_pureDirectoryName = timeName + d_orgName.right(d_orgName.length() - d_orgName.lastIndexOf('_')) + QString::fromLocal8Bit("/");
		d_orgName = orgPath + d_pureDirectoryName;
		QDir dir;
		QString currentOrgPath, currentTunedPath;

		if (d_sampleCount == 0)
		{
			currentOrgPath = d_pathA = orgPath + d_pureDirectoryName + QString::number(d_sampleCount) + QString::fromLocal8Bit("/");
		}
		else if (d_sampleCount == 1)
		{
			currentOrgPath = d_pathB = orgPath + d_pureDirectoryName + QString::number(d_sampleCount) + QString::fromLocal8Bit("/");
		}

		dir.mkpath(currentOrgPath);
		makeParameterText();
		writeParameterFile(currentOrgPath);
		currentTunedPath = d_tunedFilePath + d_pureDirectoryName + QString::number(d_sampleCount) + QString::fromLocal8Bit("/");
		dir.mkpath(currentTunedPath);
		writeParameterFile(currentTunedPath);
	}

	QString index;
	index.sprintf("%04d", int(d_graduationCount % d_graduation));
	_orgFileName = d_orgName + QString::number(d_sampleCount) + QString::fromLocal8Bit("/") + index + QString::fromLocal8Bit(".tif");
	_tunedFileName = d_tunedFilePath + d_pureDirectoryName + QString::number(d_sampleCount) + QString::fromLocal8Bit("/") + index + QString::fromLocal8Bit(".tif");
}

void ConeJointScan::sendCmdToController()
{
	ConeJointScanCmdData cmdData;
	cmdData.stsBit.s.translationCone = 0;
	cmdData.stsBit.s.currentLayer = 1;
	cmdData.stsBit.s.coneHelix = 0;
	cmdData.projectionAmount = d_framesPerGraduation * d_graduation;
	cmdData.sampleTime = d_posTime;
	cmdData.orientInc = d_orientInc;
	cmdData.circleAmount = 1;
	cmdData.centerOffset = 0;
	cmdData.firstLayerOffset = d_slicePos;
	cmdData.objRadialPos = d_sod;
	cmdData.detRadialPos = d_sdd;
	cmdData.detTransStart = d_detTransStart;
	cmdData.detTransSpace = d_detTransSpace;
	cmdData.b180Scan = 0;
	cmdData.centerOffset = 0;
	cmdData.detTransTimes = 1;
	cmdData.bSkipAirTune = 1;
	cmdData.stsBit.s.coneHelix = 0;
	d_controller->sendToControl(CMD_CONEJOINT_SCAN, (char*)(&cmdData), sizeof(ConeJointScanCmdData), false);
}

bool ConeJointScan::makeParameterText()
{
	ConeScanInterface::makeParameterText();
	auto itr = std::find(d_parameterText.begin(), d_parameterText.end(), QString::fromLocal8Bit("[GEOMETRY]\n"));
	QString panelTransPos = makeFormatQString("Translation=%.3f\t\t\t;ת̨��ת��ƽ��ƴ��ɨ��ƽ�ƾ���\n", d_detTransSpace);
	d_parameterText.insert(itr, panelTransPos);
	return false;
}

bool ConeJointScan::beginScan(int _graduation, int _framesPerGraduation, int _round, int _posTime, int _cycleTime,
	unsigned short _gainFactor, float _orientInc, float _slicePos, float _sod, float _sdd, float _detTranStart, float _detTranSpace)
{
	setCommonScanParameter(_graduation, _framesPerGraduation, _round, _posTime, _cycleTime, _gainFactor, _orientInc, _slicePos, _sod, _sdd);
	d_detTransStart = _detTranStart;
	d_detTransSpace = _detTranSpace;
	d_scanTime = 0;
	sendCmdToController();
	detachScanProcessThread();
	return true;
}




















