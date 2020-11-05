#include "stdafx.h"
#include "ctscanapp.h"
#include "ctscanwidget.h"
#include "motorwidget.h"
#include "simotioncontroller.h"
#include "linedetscanwidget.h"
#include "linedetscanmanager.h"
#include "paneldetscanmanager.h"
#include "linedetimageprocess.h"
#include "conescanwidget.h"
#include "setupdataparser.h"
#include "../PanelImageProcess/panelimageprocess.h"
#include "../Public/headers/SetupData.h"
#include "MsgListBoxDialog.h"
#include "ct3Scan.h"
#include "ct2Scan.h"
#include "drscan.h"
#include "linedetnetwork.h"
#include "acceleratorwidget.h"
#include "floatacceleratordialog.h"
#include "createDumpFile.h"
#include "../PanelDll/panel.h"

CTScanApp::CTScanApp(QWidget* d_upper, QObject *parent)
	: d_mainWidget(nullptr), d_upperWidget(d_upper)
	, d_imageWidgetManager(new ImageWidgetManager())
	, d_controller(new SimotionController())
	, d_setupData(new SetupData), d_configData(new ConfigData), d_setupDataPaser(new SetupDataParser(d_setupData.get()))
	, d_workDir(QCoreApplication::applicationDirPath())
	, d_acceleratorWidget(new AcceleratorWidget())
{
	//RunCrashHandler();
	QString time = QDateTime::currentDateTime().time().toString();
	auto i = time.length();

	for (auto index = 0; index != i; ++index)
		if (time[index] == ':')
			time[index] = '-';

	QString logFileFullName = d_workDir + "/log/" +
		QDateTime::currentDateTime().date().toString(Qt::ISODate) + '/' + time + ".txt";
	d_msg = new MsgListBoxDialog(logFileFullName);
	connect(this, &CTScanApp::infoMsgSignal, this, &CTScanApp::infoMsgSlot);
	connect(this, &CTScanApp::errorMsgSignal, this, &CTScanApp::errorMsgSlot);
	connect(this, &CTScanApp::bugMsgSignal, this, &CTScanApp::bugMsgSlot);

	LOG_INFO("软件启动");

	connect(d_controller.get(), &ControllerInterface::netWorkStsSginal
		, this, &CTScanApp::controllerNetWorkStsSlot, Qt::QueuedConnection);

	d_motorWidget = new MotorWidget(d_controller.get(), nullptr);

	for (int i = 0; i != d_setupData->lineDetNum; ++i)
	{
		int blockModuleIndex = 0;
		std::vector<unsigned int> blockModuleVec;

		while (d_setupData->lineDetData[i].nBlockMapTable[blockModuleIndex] != -1)
			blockModuleVec.push_back(d_setupData->lineDetData[i].nBlockMapTable[blockModuleIndex++]);

		std::unique_ptr<LineDetNetWork> ptr(new LineDetNetWork(d_setupData->lineDetData[i].nAcquireClientPort,
			d_setupData->lineDetData[i].nChnnelMask, d_setupData->lineDetData[i].nFIFOdepth,
			d_setupData->lineDetData[i].DelayTime, d_setupData->lineDetData[i].IntegralTime,
			d_setupData->lineDetData[i].AmplifyMultiple, blockModuleVec, d_setupData->lineDetData[i].ID));
		d_lineDetNetWorkMap.insert({ d_setupData->lineDetData[i].ID,  std::move(ptr) });
		connect(d_lineDetNetWorkMap[d_setupData->lineDetData[i].ID].get(), &LineDetNetWork::netWorkStatusSignal,
			this, &CTScanApp::lineDetNetWorkStsSlot, Qt::QueuedConnection);
	}

	d_lineDetImageProcess.reset(new LineDetImageProcess(d_workDir, d_configData->OrgPath,
		d_configData->CTPath, d_configData->DrPath));
	//查找同一种扫描方式有几种射线源和探测器组合，有几种就初始化几个线阵扫描和面阵扫描的widget
	for (auto& matrixItr : d_setupData->ct3Data)
		d_lineDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::CT3_SCAN);

	for (auto& matrixItr : d_setupData->ct2Data)
		d_lineDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::CT2_SCAN);

	for (auto& matrixItr : d_setupData->drScanData)
		d_lineDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::DR_SCAN);

	for (int i = 0; i != d_setupData->panDetNum; ++i)
	{
		int blockModuleIndex = 0;
		std::vector<unsigned int> blockModuleVec;
		std::unique_ptr<Panel> ptr(PanelFactory::getPanel(0));
		d_panelDetMap.insert({ d_setupData->panDetData[i].ID, std::move(ptr) });
	}
	
	for (auto& matrixItr : d_setupData->coneScanData)
		d_panelDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::CONE_SCAN);

	for (auto& matrixItr : d_setupData->coneJointRotScanData)
		d_panelDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::CONE_JOINT_ROT_SCAN);

	for (auto& mode : d_panelDetScanModeMap)
	{
		auto manager = new PanelDetScanManager(mode.first.first, mode.first.second,
			d_panelDetScanModeMap[mode.first], d_setupData.get(), d_panelDetMap[d_setupData->panDetData[mode.first.second].ID].get(), 
			d_controller.get(), d_imageWidgetManager.get(), nullptr);
		d_panelDetScanManagerMap[mode.first] = manager;
	}

	for (auto& mode : d_lineDetScanModeMap)
	{
		auto manager = new LineDetScanManager(mode.first.first, mode.first.second,
			d_lineDetScanModeMap[mode.first], d_setupData.get(), d_lineDetNetWorkMap[0].get(), d_controller.get());
		d_lineDetScanManagerMap[mode.first] = manager;
	}

	auto scanWidget = d_lineDetScanManagerMap[{0, 0}]->getWidget();
	d_mainWidget = new CTScanWidget(d_acceleratorWidget, scanWidget, d_motorWidget, d_upperWidget);
	connect(d_mainWidget, &CTScanWidget::showMotorButtonSignal, this, &CTScanApp::motorButonSlot);
	d_floatAcceleratorDialog = new FloatAcceleratorDialog(nullptr, Qt::FramelessWindowHint);
	QGridLayout* bLayout = new QGridLayout(d_floatAcceleratorDialog);
	auto dig = new AcceleratorWidget();
	bLayout->setContentsMargins(0, 0, 0, 0);
	d_floatAcceleratorDialog->setContentsMargins(0, 0, 0, 0);
	bLayout->addWidget(dig);

	connect(d_mainWidget, &CTScanWidget::switchToPanelWidgetSignal,
		this, &CTScanApp::switchToPanelWidgetSlot);
	connect(d_mainWidget, &CTScanWidget::switchToLineWidgetSignal,
		this, &CTScanApp::switchToLineWidgetSlot);
}

CTScanApp::~CTScanApp()
{
	LOG_INFO("软件退出");
	delete d_msg;
}

void CTScanApp::lineDetNetWorkStsSlot(int _det, bool sts)
{
	d_controller->restartLineDet(_det);
	//emit signal1(sts);测试dll信号连接
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

void CTScanApp::ctScanWidgetClosed()
{
	d_floatAcceleratorDialog->close();
}

void CTScanApp::switchToLineWidget(int _rayId, int _detId)
{
	auto widget = d_lineDetScanManagerMap[{_rayId, _detId}]->getWidget();
	d_mainWidget->switchLinePanelWidget(widget);
}

void CTScanApp::switchToPanelWidget(int _rayId, int _detId)
{
	auto widget = d_panelDetScanManagerMap[{_rayId, _detId}]->getWidget();
	d_mainWidget->switchLinePanelWidget(widget);
}

void CTScanApp::motorButonSlot()
{
	d_floatAcceleratorDialog->show();
}

void CTScanApp::switchToPanelWidgetSlot(int _rayId, int _detId)
{
	auto widget = d_panelDetScanManagerMap[{_rayId, _detId}]->getWidget();
	d_mainWidget->switchLinePanelWidget(widget);
}

void CTScanApp::switchToLineWidgetSlot(int _rayId, int _detId)
{
	auto widget = d_lineDetScanManagerMap[{_rayId, _detId}]->getWidget();
	d_mainWidget->switchLinePanelWidget(widget);
}

void CTScanApp::setMiddleWidget(QWidget * _widget)
{
}
	
void CTScanApp::setDownWidget(QWidget * _widget)
{
}
