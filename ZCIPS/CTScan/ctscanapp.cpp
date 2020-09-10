#include "stdafx.h"
#include "ctscanapp.h"
#include "ctscanwidget.h"
#include "motorcontrolwidget.h"
#include "motorwidget.h"
#include "raypanelmotionwidget.h"
#include "simotioncontroller.h"
#include "linedetscanwidget.h"
#include "conescanwidget.h"
#include "setupdataparser.h"
#include "../Public/headers/panelimageprocess.h"
#include "../Public/headers/SetupData.h"
#include "msglistbox.h"
#include "ct3Scan.h"
#include "ct2Scan.h"
#include "drscan.h"
#include "linedetnetwork.h"
#include "msglistbox.h"

CTScanApp::CTScanApp(QWidget* d_upper, QObject *parent)
	: d_mainWidget(nullptr), d_upperWidget(d_upper)
	, d_rayPanelMotion(new RayPanelMotion()), d_imageWidgetManager(new ImageWidgetManager())
	, d_controller(new SimotionController()), d_motorControl(new MotorControlWidget(d_controller.get()))
	, d_setupData(new SetupData), d_setupDataPaser(new SetupDataParser(d_setupData.get()))
	, d_workDir(QCoreApplication::applicationDirPath())
{
	QString time = QDateTime::currentDateTime().time().toString();
	auto i = time.length();

	for (auto index = 0; index != i; ++index)
		if (time[index] == ':')
			time[index] = '-';

	QString logFileFullName = d_workDir + "/log/" +
		QDateTime::currentDateTime().date().toString(Qt::ISODate) + '/' + time + ".txt";
	d_msg.reset(new MsgListBox(logFileFullName));
	connect(this, &CTScanApp::infoMsgSignal, this, &CTScanApp::infoMsgSlot, Qt::QueuedConnection);
	connect(this, &CTScanApp::errorMsgSignal, this, &CTScanApp::errorMsgSlot, Qt::QueuedConnection);
	connect(this, &CTScanApp::bugMsgSignal, this, &CTScanApp::bugMsgSlot, Qt::QueuedConnection);
	connect(d_controller.get(), &ControllerInterface::netWorkStsSginal
		, this, &CTScanApp::controllerNetWorkStsSlot, Qt::QueuedConnection);

	for (int i = 0; i != d_setupData->lineDetNum; ++i)
	{
		int blockModuleIndex = 0;
		std::vector<unsigned int> blockModuleVec;
		auto pSetupData = d_setupData.get();

		while (pSetupData->lineDetData[i].nBlockMapTable[blockModuleIndex] != -1)
			blockModuleVec.push_back(pSetupData->lineDetData[i].nBlockMapTable[blockModuleIndex++]);

		std::unique_ptr<LineDetNetWork> ptr(new LineDetNetWork(d_setupData->lineDetData[i].nAcquireClientPort,
			pSetupData->lineDetData[i].nChnnelMask, pSetupData->lineDetData[i].nFIFOdepth,
			pSetupData->lineDetData[i].DelayTime, pSetupData->lineDetData[i].IntegralTime,
			pSetupData->lineDetData[i].AmplifyMultiple, blockModuleVec, pSetupData->lineDetData[i].ID));
		d_lineDetNetWorkMap.insert({ d_setupData->lineDetData[i].ID,  std::move(ptr) });
		connect(d_lineDetNetWorkMap[d_setupData->lineDetData[i].ID].get(), &LineDetNetWork::netWorkStatusSignal,
			this, &CTScanApp::lineDetNetWorkStsSlot);
	}

	//查找同一种扫描方式有几种射线源和探测器组合，有几种就初始化几个线阵扫描和面阵扫描的widget
	for (auto& matrixItr : d_setupData->ct3Data)
		d_lineDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::CT3_SCAN);

	for (auto& matrixItr : d_setupData->ct2Data)
		d_lineDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::CT2_SCAN);

	for (auto& matrixItr : d_setupData->drScanData)
		d_lineDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::DR_SCAN);

	for (auto& matrixItr : d_setupData->coneScanData)
		d_panelDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::CONE_SCAN);

	for (auto& matrixItr : d_setupData->coneJointRotScanData)
		d_panelDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::CONE_JOINT_ROT_SCAN);

	for (auto& mode : d_panelDetScanModeMap)
	{
		auto widget = new ConeScanWidget(d_motorControl.get(), mode.first.first, mode.first.second,
			d_panelDetScanModeMap[mode.first], d_setupData.get(), nullptr, d_controller.get(), nullptr);
		d_panelDetScanWidget[mode.first] = widget;
	}

	for (auto& mode : d_lineDetScanModeMap)
	{
		auto widget = new LineDetScanWidget(d_motorControl.get(), mode.first.first, mode.first.second,
			d_lineDetScanModeMap[mode.first], d_setupData.get(), d_lineDetNetWorkMap[0].get(), d_controller.get(), nullptr);
		d_lineDetScanWidget[mode.first] = widget;
	}

	auto scanWidget = d_lineDetScanWidget[{0, 0}];
	d_motorWidget = new MotorWidget(d_controller.get(), nullptr);
	d_mainWidget = new CTScanWidget(scanWidget, d_motorWidget, d_upperWidget);
}

CTScanApp::~CTScanApp()
{
}

void CTScanApp::lineDetNetWorkStsSlot(int _det, bool sts)
{
	d_controller->restartLineDet(_det);
	LOG_INFO("重启线阵探测器");
}

void CTScanApp::controllerNetWorkStsSlot(bool sts)
{

}
void CTScanApp::errorMsgSlot(QString msg)
{
	d_msg->logError(msg);
}

void CTScanApp::infoMsgSlot(QString msg)
{
	d_msg->logInfo(msg);
}

void CTScanApp::bugMsgSlot(QString msg)
{
	d_msg->logBug(msg);
}

void CTScanApp::setMiddleWidget(QWidget * _widget)
{
}
	
void CTScanApp::setDownWidget(QWidget * _widget)
{
}
