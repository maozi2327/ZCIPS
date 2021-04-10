#include "stdafx.h"
#include "ctscanapp.h"
#include "ctscanwidget.h"
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
#include "axiscontrolwidget.h"
#include "axiszerocoordinationdialog.h"
#include "axisstatusdialog.h"
#include "../PanelDetector/panel.h"
#include "linedetdebugdialog.h"
#include "filepathsettingdialog.h"
#include "tipdialog.h"
#include "colimatecontroller.h"
#include "colimatezeroajustdialog.h"
#include "colimatecontroldialog.h"

QString str(QCoreApplication::applicationDirPath());

CTScanApp::CTScanApp(QWidget* d_upper, QObject *parent)
	: d_mainWidget(nullptr), d_upperWidget(d_upper)
	, d_imageWidgetManager(new ImageDialogManager())
	, d_setupData(new SetupData), d_configData(new ConfigData), d_setupDataPaser(new SetupDataParser(d_setupData.get()))
	, d_workDir(QCoreApplication::applicationDirPath() + QString::fromLocal8Bit("/"))
	, d_acceleratorWidget(new AcceleratorWidget()), d_tipDialog(new TipDialog), d_colimateController(new ColimateController(d_workDir))
{
	//RunCrashHandler();
	getAppSettingDataFromFile();
	QString time = QDateTime::currentDateTime().time().toString();
	auto i = time.length();

	for (auto index = 0; index != i; ++index)
		if (time[index] == ':')
			time[index] = '-';

	QString logFileFullName = d_workDir + "log/" +
		QDateTime::currentDateTime().date().toString(Qt::ISODate) + '/' + time + ".txt";
	d_msg = new MsgListBoxDialog(logFileFullName);
	connect(this, &CTScanApp::infoMsgSignal, this, &CTScanApp::infoMsgSlot);
	connect(this, &CTScanApp::errorMsgSignal, this, &CTScanApp::errorMsgSlot);
	connect(this, &CTScanApp::bugMsgSignal, this, &CTScanApp::bugMsgSlot);
	connect(d_controller.get(), &ControllerInterface::netWorkStsSginal
		, this, &CTScanApp::controllerNetWorkStsSlot, Qt::QueuedConnection);

	for (auto &axisDefineItr : d_setupData->sysAxisDefine)
	{
		auto axisDataItr = AxisDataMap.find(axisDefineItr);
		
		if (axisDataItr != AxisDataMap.end())
			d_axisDataMap[axisDefineItr] = axisDataItr->second;
		else
			LOG_ERROR("δ�������");
	}

	d_controller.reset(new SimotionController(d_axisDataMap));
	d_axisStatusWidget = new AxisControlwidget(d_controller.get(), d_axisDataMap);

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

	d_lineDetImageProcess.reset(new LineDetImageProcess(d_workDir));
	//����ͬһ��ɨ�跽ʽ�м�������Դ��̽������ϣ��м��־ͳ�ʼ����������ɨ�������ɨ���widget
	for (auto& matrixItr : d_setupData->ct3Data)
		d_lineDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::CT3_SCAN);

	for (auto& matrixItr : d_setupData->ct2Data)
		d_lineDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::CT2_SCAN);

	for (auto& matrixItr : d_setupData->drScanData)
		d_lineDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::DR_SCAN);

	for (int i = 0; i != d_setupData->panDetNum; ++i)
	{
		std::unique_ptr<Panel> ptr(PanelFactory::getPanel(0));
		ptr->setPanelSize(d_setupData->panDetData[i].horizontalPixels, d_setupData->panDetData[i].verticalPixels);
		d_panelDetMap.insert({ d_setupData->panDetData[i].ID, std::move(ptr) });
	}
	
	for (auto& matrixItr : d_setupData->coneScanData)
		d_panelDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::CONE_SCAN);

	for (auto& matrixItr : d_setupData->coneJointRotScanData)
		d_panelDetScanModeMap[{ matrixItr.Ray, matrixItr.Det }].push_back(ScanMode::CONE_JOINT_ROT_SCAN);

	d_tunedPanelBkgPath = d_workDir + QString::fromLocal8Bit("correctedPanel/bkg/");
	d_tunedPanelAirPath = d_workDir + QString::fromLocal8Bit("correctedPanel/air/");
	d_defectFileName = d_workDir + QString::fromLocal8Bit("defect.dat");

	for (auto& mode : d_panelDetScanModeMap)
	{
		auto manager = new PanelDetScanManager(mode.first.first, mode.first.second,
			d_panelDetScanModeMap[mode.first], d_setupData.get(), d_panelDetMap[d_setupData->panDetData[mode.first.second].ID].get(), 
			d_controller.get(), d_imageWidgetManager.get(), d_tunedFilePath, d_orgPath, d_tunedPanelBkgPath, d_tunedPanelAirPath, d_defectFileName, nullptr);
		d_panelDetScanManagerMap[mode.first] = manager;
		connect(manager, &PanelDetScanManager::objectNameNumberChangedSignal, this, &CTScanApp::objectNameNumberChangedSlot);
	}

	for (auto& mode : d_lineDetScanModeMap)
	{
		auto manager = new LineDetScanManager(mode.first.first, mode.first.second,
			d_lineDetScanModeMap[mode.first], d_setupData.get(), d_lineDetNetWorkMap[0].get(), d_controller.get(), d_lineDetImageProcess.get(),
			d_tunedFilePath, d_orgPath);
		d_lineDetScanManagerMap[mode.first] = manager;
		connect(manager, &LineDetScanManager::objectNameNumberChangedSignal, this, &CTScanApp::objectNameNumberChangedSlot);
	}

	QWidget* scanWidget = nullptr;

	if (d_appSettingData.detType == ARC_DETECTOR)
		scanWidget = d_lineDetScanManagerMap[{d_appSettingData.rayId, 0}]->getWidget();
	else if (d_appSettingData.detType == PANEL_DETECTOR)
		scanWidget = d_panelDetScanManagerMap[{d_appSettingData.rayId, d_appSettingData.detId}]->getWidget();
	
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

	buildMenuBar();
	d_timer = new QTimer();
	d_timer->start(200);	
	connect(d_timer, &QTimer::timeout, this, &CTScanApp::updateSystemStatusSlot);
	LOG_INFO("�������");
}

CTScanApp::~CTScanApp()
{
	LOG_INFO("����˳�");
	delete d_msg;
}

void CTScanApp::lineDetNetWorkStsSlot(int _det, bool sts)
{
	d_controller->restartLineDet(_det);
	//emit signal1(sts);����dll�ź�����
	LOG_INFO("��������̽����");
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
	d_appSettingData.detId = _detId;
	d_appSettingData.rayId = _rayId;
	d_appSettingData.detType = ARC_DETECTOR;
	saveAppSettingData();
}

void CTScanApp::switchToPanelWidget(int _rayId, int _detId)
{
	auto widget = static_cast<ConeScanWidget*>(d_panelDetScanManagerMap[{_rayId, _detId}]->getWidget());
	d_mainWidget->switchLinePanelWidget(widget);
	d_appSettingData.detId = _detId;
	d_appSettingData.rayId = _rayId;
	d_appSettingData.detType = PANEL_DETECTOR;
	saveAppSettingData();
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
	
	//ϵͳ�˵�
	d_systemMenu = new QMenu(d_menuBar);
	d_systemMenu->setObjectName(QStringLiteral("systemMenu"));
	d_systemMenu->setTitle(QString::fromLocal8Bit("ϵͳ"));
	d_menuBar->addAction(d_systemMenu->menuAction());

	d_initiliseSystemAction = new QAction(this);
	d_initiliseSystemAction->setObjectName(QStringLiteral("initiliseSystemAction"));
	d_initiliseSystemAction->setText(QString::fromLocal8Bit("��ʼ��"));
	d_systemMenu->addAction(d_initiliseSystemAction);

	d_filePathSettingAction = new QAction(this);
	d_filePathSettingAction->setObjectName(QStringLiteral("filePathSettingAction"));
	d_filePathSettingAction->setText(QString::fromLocal8Bit("ͼ���ļ�����·��"));
	d_systemMenu->addAction(d_filePathSettingAction);

	d_rayDetSwitchMenu = new QMenu(d_systemMenu);
	d_rayDetSwitchMenu->setTitle(QString::fromLocal8Bit("����Դ-̽��������л�"));
	d_systemMenu->addAction(d_rayDetSwitchMenu->menuAction());
	
	d_colimateControlAction = new QAction(d_systemMenu);
	d_colimateControlAction->setObjectName(QStringLiteral("colimateControlAction"));
	d_colimateControlAction->setText(QString::fromLocal8Bit("׼ֱ��/���ſ���"));
	d_systemMenu->addAction(d_colimateControlAction);;

	for (auto& itr : d_lineDetScanModeMap)
	{
		auto action = new QAction(this);
		d_lineDetRayActions[itr.first] = action;
		QString text(QString::fromLocal8Bit(d_setupData->kVRayData[itr.first.first].rayType)
			+ QString::fromLocal8Bit("--") + QString::fromLocal8Bit("����̽����"));
		action->setText(text);
		d_rayDetSwitchMenu->addAction(action);
	}

	for (auto& itr : d_panelDetScanModeMap)
	{
		auto action = new QAction(this);
		d_panelDetRayActions[itr.first] = action;
		QString text(QString::fromLocal8Bit(d_setupData->kVRayData[itr.first.first].rayType) +
			QString::fromLocal8Bit("--") +
			QString::fromLocal8Bit("����̽����") + 
			QString::fromLocal8Bit(d_setupData->panDetData[itr.first.second].PandetType));
		action->setText(text);
		d_rayDetSwitchMenu->addAction(action);
	}

	d_viewMenu = new QMenu(d_menuBar);
	d_viewMenu->setObjectName(QStringLiteral("viewMenu"));
	d_viewMenu->setTitle(QString::fromLocal8Bit("��ͼ"));
	d_menuBar->addAction(d_viewMenu->menuAction());

	d_axisStatusAction = new QAction(this);
	d_axisStatusAction->setObjectName(QStringLiteral("axisStatusAction"));
	d_axisStatusAction->setText(QString::fromLocal8Bit("��״̬"));
	d_viewMenu->addAction(d_axisStatusAction);

	connect(d_initiliseSystemAction, &QAction::triggered, this, &CTScanApp::on_initiliseSystemAction_triggered);
	connect(d_filePathSettingAction, &QAction::triggered, this, &CTScanApp::on_filePathSettingAction_triggered);
	connect(d_axisStatusAction, &QAction::triggered, this, &CTScanApp::on_axisStatusAction_triggered);
	connect(d_colimateControlAction, &QAction::triggered, this, &CTScanApp::on_colimateControlAction_triggered);

	//���Բ˵�
	if (d_debugSystem)
	{
		d_debugMenu = new QMenu(d_menuBar);
		d_debugMenu->setObjectName(QStringLiteral("systemMenu"));
		d_debugMenu->setTitle(QString::fromLocal8Bit("����"));
		d_menuBar->addAction(d_debugMenu->menuAction());

		d_axisZeroCoordinateAction = new QAction(this);
		d_axisZeroCoordinateAction->setObjectName(QStringLiteral("axisZeroCoordinateAction"));
		d_axisZeroCoordinateAction->setText(QString::fromLocal8Bit("���궨"));
		d_debugMenu->addAction(d_axisZeroCoordinateAction);
		connect(d_axisZeroCoordinateAction, &QAction::triggered, this, &CTScanApp::on_axisZeroCoordinateAction_triggered);

		d_axisSpeedAction = new QAction(this);
		d_axisSpeedAction->setObjectName(QStringLiteral("d_axisSpeedAction"));
		d_axisSpeedAction->setText(QString::fromLocal8Bit("���ٶ�"));
		d_debugMenu->addAction(d_axisSpeedAction);
		connect(d_axisSpeedAction, &QAction::triggered, this, &CTScanApp::on_axisSpeedAction_triggered);

		d_lineDetectorAction = new QAction(this);
		d_lineDetectorAction->setObjectName(QStringLiteral("lineDetectorAction"));
		d_lineDetectorAction->setText(QString::fromLocal8Bit("����̽�������ò鿴"));
		d_debugMenu->addAction(d_lineDetectorAction);
		connect(d_lineDetectorAction, &QAction::triggered, this, &CTScanApp::on_lineDetectorAction_triggered);

		d_autoAlignLayerAction = new QAction(this);
		d_autoAlignLayerAction->setObjectName(QStringLiteral("d_autoAlignLayerAction"));
		d_autoAlignLayerAction->setText(QString::fromLocal8Bit("�����Զ�����"));
		d_debugMenu->addAction(d_autoAlignLayerAction);
		connect(d_autoAlignLayerAction, &QAction::triggered, this, &CTScanApp::on_autoAlignLayerAction_triggered);

		d_laserInterferometerAction = new QAction(this);
		d_laserInterferometerAction->setObjectName(QStringLiteral("autoAlignLayer"));
		d_laserInterferometerAction->setText(QString::fromLocal8Bit("��������ǳ���"));
		d_debugMenu->addAction(d_laserInterferometerAction);
		connect(d_laserInterferometerAction, &QAction::triggered, this, &CTScanApp::on_laserInterferometerAction_triggered);

		d_colimateZeroAjustAction = new QAction(this);
		d_colimateZeroAjustAction->setObjectName(QStringLiteral("colimateZeroAjustAction"));
		d_colimateZeroAjustAction->setText(QString::fromLocal8Bit("׼ֱ�����궨"));
		d_debugMenu->addAction(d_colimateZeroAjustAction);
		connect(d_colimateZeroAjustAction, &QAction::triggered, this, &CTScanApp::on_colimateZeroAjustAction_triggered);
	}
}

bool CTScanApp::saveAppSettingData()
{
	QFile file;
	file.setFileName(d_appSettingDataFileName);

	if (!file.open(QIODevice::WriteOnly))
	{
		LOG_ERROR("��Ӧ�������ļ�ʧ��");
		return false;
	}

	file.write((char*)&d_appSettingData, sizeof(d_appSettingData));
	return true;
}

bool CTScanApp::getAppSettingDataFromFile()
{
	QFile file;
	file.setFileName(d_appSettingDataFileName);

	if (!file.open(QIODevice::ReadOnly))
	{
		LOG_ERROR("��Ӧ�������ļ�ʧ��");
		d_appSettingData.detType = ARC_DETECTOR;
		d_appSettingData.detId = 0;
		d_appSettingData.rayId = 0;
		on_filePathSettingAction_triggered();
		return false;
	}

	auto byteArray = file.readAll();
	memcpy(&d_appSettingData, byteArray.data(), sizeof(d_appSettingData));
	QDir path;
	d_orgPath = QString::fromLocal8Bit(d_appSettingData.orgPath);
	d_tunedFilePath = QString::fromLocal8Bit(d_appSettingData.tuneFilePath);

	if(!path.exists(d_orgPath) || !path.exists(d_tunedFilePath))
		on_filePathSettingAction_triggered();

	return true;
}

void CTScanApp::on_axisZeroCoordinateAction_triggered()
{
	if(d_axisZeroCoordinationDialog == nullptr)
		d_axisZeroCoordinationDialog = new AxisZeroCoordinationDialog(d_controller.get(), d_axisDataMap, 
			d_mainWidget);

	d_axisZeroCoordinationDialog->exec();
}

void CTScanApp::on_axisSpeedAction_triggered()
{

}

void CTScanApp::on_lineDetectorAction_triggered()
{
	LineDetDebugDialog dlg(d_lineDetNetWorkMap[0].get(), d_setupData->lineDetData[0].nChnnelMask, 
		d_setupData->lineDetData[0].nFIFOdepth, d_setupData->lineDetData[0].DelayTime, 
		d_setupData->lineDetData[0].IntegralTime, d_setupData->lineDetData[0].AmplifyMultiple);
	dlg.exec();
}

void CTScanApp::on_autoAlignLayerAction_triggered()
{
}
void CTScanApp::on_laserInterferometerAction_triggered()
{
}

void CTScanApp::on_colimateZeroAjustAction_triggered()
{
	ColimateZeroAjustDialog d_colimateZeroAjustDialog(d_colimateController->getMotor(), d_mainWidget);
	d_colimateZeroAjustDialog.exec();
}

void CTScanApp::updateSystemStatusSlot()
{
	bool controlConnected = d_controller->getConnected();
	bool idle = d_controller->readIdleStatus();
	bool ready = d_controller->readReadyStatus();

	int rayId = d_appSettingData.rayId;
	int detId;

	//TODO_DJ�������������������Ӧ���
	if (d_appSettingData.detType == ARC_DETECTOR)
		detId = 0;
	else if(d_appSettingData.detType == PANEL_DETECTOR)
		detId = 1;

	d_controller->sendRayDetCoupleStatus(rayId, detId);
	d_controller->getAxisPosition();

	for (auto& scanManager : d_lineDetScanManagerMap)
	{
		scanManager.second->updateStatus(controlConnected && idle && ready && d_lineDetNetWorkMap[scanManager.first.second]->getConnected());
	}

	for (auto& scanManager : d_panelDetScanManagerMap)
	{
		scanManager.second->updateControlSystemStatus(controlConnected && idle && ready);
		scanManager.second->updatePanelStatus(d_panelDetMap[scanManager.first.second]->getConnected() && idle);
	}

	//QString tip;

	//if (!controlConnected)
	//	tip += QString::fromLocal8Bit("����ϵͳδ���ӣ�\n");
	//else
	//{
	//	if (!ready)
	//		tip += QString::fromLocal8Bit("����ϵͳδ��ʼ����\n");
	//	if (!idle)
	//		tip += QString::fromLocal8Bit("ϵͳæ��\n");
	//}

	auto importantSts = d_controller->readImportantStatus();
	std::vector<QString> tip;

	if(!importantSts[ControllerInterface::network])
		tip.push_back(QString::fromLocal8Bit("�˶�ϵͳͨ�ŶϿ�!"));
	else
	{
		if (importantSts[ControllerInterface::seekingZero])
			tip.push_back(QString::fromLocal8Bit("���ڳ�ʼ��!"));
		if (!d_controller->readReadyStatus())
			tip.push_back(QString::fromLocal8Bit("ϵͳδ��ʼ��!"));
		if (importantSts[ControllerInterface::collide])
			tip.push_back(QString::fromLocal8Bit("С����ײ!"));
		if (importantSts[ControllerInterface::sliceThickAdjusting])
			tip.push_back(QString::fromLocal8Bit("�����л����!"));
		if (importantSts[ControllerInterface::rayDetCoupleNotSame])
			tip.push_back(QString::fromLocal8Bit("����Դ̽������ϲ�һ�£����л�����Դ̽�������!"));
		if (importantSts[ControllerInterface::changingDet])
			tip.push_back(QString::fromLocal8Bit("�����л�̽����!"));
		if (importantSts[ControllerInterface::changingRay])
			tip.push_back(QString::fromLocal8Bit("�����л�����Դ!"));
		if (importantSts[ControllerInterface::busy])
			tip.push_back(QString::fromLocal8Bit("ϵͳæ!"));
	}

	d_tipDialog->setTip(tip);
	//if (!detConnected)
	//	tip += QString::fromLocal8Bit("̽����δ���ӣ�\n");
}

void CTScanApp::objectNameNumberChangedSlot(const QString& _objectName, const QString& _objectNumber)
{
	d_objectName = _objectName;
	d_objectNumber = _objectNumber;
	
	for (auto& manager : d_lineDetScanManagerMap)
		manager.second->updateObjectNameNumber(d_objectName, d_objectNumber);

	for (auto& manager : d_panelDetScanManagerMap)
		manager.second->updateObjectNameNumber(d_objectName, d_objectNumber);
}

void CTScanApp::on_initiliseSystemAction_triggered()
{
	d_controller->initialiseController();
}

void CTScanApp::on_filePathSettingAction_triggered()
{
	FilePathSettingDialog dialog(d_orgPath, d_tunedFilePath, nullptr);
	int ret = dialog.exec();

	if (ret == QDialog::Accepted)
	{
		if (*d_orgPath.rbegin() != QChar::fromLatin1('/'))
			d_orgPath += QString::fromLocal8Bit("/");

		QByteArray byteArray = d_orgPath.toLocal8Bit();
		strcpy(d_appSettingData.orgPath, byteArray.data());

		if (*d_tunedFilePath.rbegin() != QChar::fromLatin1('/'))
			d_tunedFilePath += QString::fromLocal8Bit("/");

		byteArray = d_tunedFilePath.toLocal8Bit();
		strcpy(d_appSettingData.tuneFilePath, byteArray.data());
		saveAppSettingData();

		for (auto& i : d_lineDetScanManagerMap)
			i.second->updateFileDirectory(d_orgPath, d_tunedFilePath);
	}
}

void CTScanApp::on_rayDetAction_triggered()
{
	
}

void CTScanApp::on_axisStatusAction_triggered()
{
	if(AxisStatusDialog::getDialogCounter() == 0)
		d_axisStatusDialog = new AxisStatusDialog(d_controller.get(), d_axisDataMap, d_mainWidget);

	d_axisStatusDialog->setAttribute(Qt::WA_DeleteOnClose, true);
	d_axisStatusDialog->show();
}

void CTScanApp::on_colimateControlAction_triggered()
{
	if (ColimateControlDialog::getDialogCount() == 0)
		d_colimateControlDialog = new ColimateControlDialog(d_colimateController->getMotor(), d_mainWidget);

	d_colimateControlDialog->setAttribute(Qt::WA_DeleteOnClose, true);
	d_colimateControlDialog->show();
}

void CTScanApp::motorButonSlot()
{
	d_floatAcceleratorDialog->show();
}

void CTScanApp::switchToPanelWidgetSlot(int _rayId, int _detId)
{
	switchToPanelWidget(_rayId, _detId);
	int rayDetCouple = 1;

	if (_rayId == 0 && _detId == 0)
		rayDetCouple = 1;
	else if(_rayId == 1 && _detId == 0)
		rayDetCouple = 3;

	d_controller->switchRayDetCouple(rayDetCouple);
	auto height = d_menuBar->height() + d_mainWidget->height() + d_mainWidget->layout()->contentsMargins().top();
	emit sizeChangedSignal(d_menuBar->height() + d_mainWidget->height() + d_mainWidget->layout()->contentsMargins().top());
}

void CTScanApp::switchToLineWidgetSlot(int _rayId, int _detId)
{
	switchToLineWidget(_rayId, _detId);
	int rayDetCouple = 0;

	if (_rayId == 0 && _detId == 0)
		rayDetCouple = 0;
	else if (_rayId == 1 && _detId == 0)
		rayDetCouple = 2;

	d_controller->switchRayDetCouple(rayDetCouple);
	emit sizeChangedSignal(d_menuBar->height() + d_mainWidget->height() + d_mainWidget->layout()->contentsMargins().top());
}

void CTScanApp::setMiddleWidget(QWidget * _widget)
{
}
	
void CTScanApp::setDownWidget(QWidget * _widget)
{
}

