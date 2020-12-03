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
#include "imagedialogmanager.h"
#include "axisstatuswidget.h"
#include "axiszerocoordinationdialog.h"
#include "../PanelDll/panel.h"

CTScanApp::CTScanApp(QWidget* d_upper, QObject *parent)
	: d_mainWidget(nullptr), d_upperWidget(d_upper)
	, d_imageWidgetManager(new ImageDialogManager())
	, d_controller(new SimotionController())
	, d_setupData(new SetupData), d_configData(new ConfigData), d_setupDataPaser(new SetupDataParser(d_setupData.get()))
	, d_workDir(QCoreApplication::applicationDirPath())
	, d_acceleratorWidget(new AcceleratorWidget())
{
	buildMenuBar();
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
	connect(d_controller.get(), &ControllerInterface::netWorkStsSginal
		, this, &CTScanApp::controllerNetWorkStsSlot, Qt::QueuedConnection);

	d_motorWidget = new MotorWidget(d_controller.get(), nullptr);

	for (auto &axisDefineItr : d_setupData->sysAxisDefine)
	{
		auto axisDataItr = AxisDataMap.find(axisDefineItr);
		
		if (axisDataItr != AxisDataMap.end())
			d_axisDataMap[axisDefineItr] = axisDataItr->second;
		else
			LOG_ERROR("未定义的轴");
	}
	
	d_axisStatusWidget = new AxisStatusWidget(d_controller.get(), d_axisDataMap);
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
	d_mainWidget = new CTScanWidget(d_acceleratorWidget, scanWidget, d_axisStatusWidget, d_upperWidget);
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

	LOG_INFO("软件启动");
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
	auto widget = static_cast<ConeScanWidget*>(d_panelDetScanManagerMap[{_rayId, _detId}]->getWidget());
	d_mainWidget->switchLinePanelWidget(widget);
}

QMenuBar * CTScanApp::getMenuBar()
{
	return d_menuBar;
}

void CTScanApp::buildMenuBar()
{
	d_menuBar = new QMenuBar();
	d_menuBar->setObjectName(QStringLiteral("menuBar"));
	d_menuBar->setGeometry(QRect(0, 0, 342, 23));
	
	//系统菜单
	d_systemMenu = new QMenu(d_menuBar);
	d_systemMenu->setObjectName(QStringLiteral("systemMenu"));
	d_systemMenu->setTitle(QString::fromLocal8Bit("系统"));
	d_menuBar->addAction(d_systemMenu->menuAction());

	d_initiliseSystemAction = new QAction(this);
	d_initiliseSystemAction->setObjectName(QStringLiteral("initiliseSystemAction"));
	d_initiliseSystemAction->setText(QString::fromLocal8Bit("初始化"));
	d_systemMenu->addAction(d_initiliseSystemAction);

	connect(d_initiliseSystemAction, &QAction::triggered, this, &CTScanApp::on_initiliseSystemAction_triggered);

	//调试菜单
	if (d_debugSystem)
	{
		d_debugMenu = new QMenu(d_menuBar);
		d_debugMenu->setObjectName(QStringLiteral("systemMenu"));
		d_debugMenu->setTitle(QString::fromLocal8Bit("调试"));
		d_menuBar->addAction(d_debugMenu->menuAction());

		d_axisZeroCoordinateAction = new QAction(this);
		d_axisZeroCoordinateAction->setObjectName(QStringLiteral("axisZeroCoordinateAction"));
		d_axisZeroCoordinateAction->setText(QString::fromLocal8Bit("零点标定"));
		d_debugMenu->addAction(d_axisZeroCoordinateAction);
		connect(d_axisZeroCoordinateAction, &QAction::triggered, this, &CTScanApp::on_axisZeroCoordinateAction_triggered);

		d_axisSpeedAction = new QAction(this);
		d_axisSpeedAction->setObjectName(QStringLiteral("d_axisSpeedAction"));
		d_axisSpeedAction->setText(QString::fromLocal8Bit("轴速度"));
		d_debugMenu->addAction(d_axisSpeedAction);
		connect(d_axisSpeedAction, &QAction::triggered, this, &CTScanApp::on_axisSpeedAction_triggered);

		d_lineDetectorAction = new QAction(this);
		d_lineDetectorAction->setObjectName(QStringLiteral("lineDetectorAction"));
		d_lineDetectorAction->setText(QString::fromLocal8Bit("线阵探测器设置查看"));
		d_debugMenu->addAction(d_lineDetectorAction);
		connect(d_lineDetectorAction, &QAction::triggered, this, &CTScanApp::on_lineDetectorAction_triggered);

		d_autoAlignLayerAction = new QAction(this);
		d_autoAlignLayerAction->setObjectName(QStringLiteral("d_autoAlignLayerAction"));
		d_autoAlignLayerAction->setText(QString::fromLocal8Bit("射线自动对齐"));
		d_debugMenu->addAction(d_autoAlignLayerAction);
		connect(d_autoAlignLayerAction, &QAction::triggered, this, &CTScanApp::on_autoAlignLayerAction_triggered);

		d_laserInterferometerAction = new QAction(this);
		d_laserInterferometerAction->setObjectName(QStringLiteral("autoAlignLayer"));
		d_laserInterferometerAction->setText(QString::fromLocal8Bit("激光干涉仪程序"));
		d_debugMenu->addAction(d_laserInterferometerAction);
		connect(d_laserInterferometerAction, &QAction::triggered, this, &CTScanApp::on_laserInterferometerAction_triggered);
	}
}
void CTScanApp::on_axisZeroCoordinateAction_triggered()
{
	d_axisZeroCoordinationDialog = new AxisZeroCoordinationDialog(d_controller.get(), d_axisDataMap);
	d_axisZeroCoordinationDialog->show();
}
void CTScanApp::on_axisSpeedAction_triggered()
{
}
void CTScanApp::on_lineDetectorAction_triggered()
{
}
void CTScanApp::on_autoAlignLayerAction_triggered()
{
}
void CTScanApp::on_laserInterferometerAction_triggered()
{
}
void CTScanApp::on_initiliseSystemAction_triggered()
{
	d_controller->initialiseController();
}

void CTScanApp::motorButonSlot()
{
	d_floatAcceleratorDialog->show();
}

void CTScanApp::switchToPanelWidgetSlot(int _rayId, int _detId)
{
	auto widget = static_cast<ConeScanWidget*>(d_panelDetScanManagerMap[{_rayId, _detId}]->getWidget());
	widget->setPanelDetWidget();
	d_mainWidget->switchLinePanelWidget(widget);
	//d_panelDetMap[0]->getWidget()->show();
}

void CTScanApp::switchToLineWidgetSlot(int _rayId, int _detId)
{
	auto widget = d_lineDetScanManagerMap[{_rayId, _detId}]->getWidget();
	d_mainWidget->switchLinePanelWidget(widget);
	//d_panelDetMap[0]->getWidget()->show();
}

void CTScanApp::setMiddleWidget(QWidget * _widget)
{
}
	
void CTScanApp::setDownWidget(QWidget * _widget)
{
}
