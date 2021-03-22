#include "stdafx.h"
#include "linedetscanmanager.h"
#include "linedetscanwidget.h"
#include "simotioncontroller.h"
#include "LineDetNetWork.h"
#include "../Public/headers/setupData.h"
#include "ct3scan.h"
#include "linedetbkgtune.h"
#include "linedetairtune.h"
#include "CT3TemplateDialog.h"
#include "linedetairdisposedialog.h"
#include "scantemplate.h"
#include "linedetfilenamedialog.h"

QWidget * LineDetScanManager::getWidget()
{
	return d_lineDetScanWidget;
}

LineDetScanManager::LineDetScanManager(int _rayId, int _lineDetId, const std::vector<ScanMode>& _scanMode, 
	SetupData* _setupData, LineDetNetWork* _lineDetNetWork, ControllerInterface* _controller, /*
	const QString& _initialDirectory, const QString& _orgDirectory, const QString& _objectName, 
	const QString& _objectNumber, */QWidget* _widgetParent, QObject *_ObjectParent)
	: QObject(_ObjectParent)
	, d_rayNum(_rayId), d_detNum(_lineDetId)
	, d_setupData(_setupData), d_lineDetNetWork(_lineDetNetWork), d_controller(_controller)
	, d_lineDetScanWidget(new LineDetScanWidget(_widgetParent))
{
	for (auto& scanMode : _scanMode)
	{
		if (scanMode == ScanMode::CT3_SCAN)
		{
			auto itr = std::find_if(d_setupData->ct3Data.cbegin(), d_setupData->ct3Data.cend(),
				[=](const CT3Data& _ct3Data)
			{	return _ct3Data.Ray == _rayId && _ct3Data.Det == _lineDetId; });

			d_lineDetScanWidget->initiliseCt3Controls(*itr);
			d_ct3Data = *itr;
		}
		else if (scanMode == ScanMode::DR_SCAN)
		{
			auto itr = std::find_if(d_setupData->drScanData.cbegin(), d_setupData->drScanData.cend(),
				[=](const DrScanData& _drData)
			{	return _drData.Ray == _rayId && _drData.Det == _lineDetId; });

			d_lineDetScanWidget->initiliseDrControls(*itr);
		}
		else if (scanMode == ScanMode::CT2_SCAN)
		{
			auto itr = std::find_if(d_setupData->ct2Data.cbegin(), d_setupData->ct2Data.cend(),
				[=](const CT2Data& _ct2Data)
			{	return _ct2Data.Ray == _rayId && _ct2Data.Det == _lineDetId; });

			d_lineDetScanWidget->initiliseCt2Controls(*itr);
		}
	}

	connect(d_lineDetScanWidget, &LineDetScanWidget::ct3ScanSignal, 
		this, &LineDetScanManager::ct3ScanSlot);
	connect(d_lineDetScanWidget, &LineDetScanWidget::airTuneSignal,
		this, &LineDetScanManager::airTuneSlot);
	connect(d_lineDetScanWidget, &LineDetScanWidget::loadTuneDataSignal,
		this, &LineDetScanManager::loadTuneDataSlot);
	connect(d_lineDetScanWidget, &LineDetScanWidget::LoadCt3TemplateButtonSignal, 
		this, &LineDetScanManager::LoadCt3TemplateButtonSlot);
	d_timer = new QTimer();
	d_timer->start(1000);
	connect(d_timer, &QTimer::timeout, this, &LineDetScanManager::updateControlsSlot);
}

LineDetScanManager::~LineDetScanManager()
{
}

void LineDetScanManager::ct3ScanSlot()
{
	QString objectName, objectNumber, fileNumber, comment;
	getFileNameFromDialog(objectNumber, objectNumber, fileNumber, comment);

	float layer = d_lineDetScanWidget->getLayer();
	int matrix = d_lineDetScanWidget->ui.ct3MatrixComboBox->currentText().toInt();
	int view = d_lineDetScanWidget->ui.ct3ViewComboBox->currentText().toInt();
	int sampleTime = d_lineDetScanWidget->ui.ct3SampleTimeComboBox->currentText().toInt();
	float angle = d_lineDetScanWidget->ui.ct3AngleLineEdit->text().toFloat();
	d_scan.reset(new CT3Scan(d_controller, d_lineDetNetWork, d_setupData, d_detNum));

	if (!dynamic_cast<CT3Scan*>(d_scan.get())
		->setScanParameter(layer, matrix, view, sampleTime, angle))
		return;

	QString fileName = d_initialFileDirectory + objectName + '/' + objectNumber + '/' + fileNumber + 
		QString::fromLocal8Bit("_") + comment + QString::fromLocal8Bit(".org");
	QString destFilePath = d_orgDirectory + objectName + '/' + objectNumber + '/';
	d_scan->setFileName(fileName, destFilePath);

	connect(d_scan.get(), &CT3Scan::signalGraduationCount, d_lineDetScanWidget, 
		&LineDetScanWidget::updateCT3Progresser);
	d_scan->beginScan();
}

void LineDetScanManager::drScanSlot()
{
	QString objectName, objectNumber, fileNumber, comment;
	getFileNameFromDialog(objectNumber, objectNumber, fileNumber, comment);
	
	int matrix = d_lineDetScanWidget->ui.ct3MatrixComboBox->currentText().toInt();
	int view = d_lineDetScanWidget->ui.ct3ViewComboBox->currentText().toInt();
	int sampleTime = d_lineDetScanWidget->ui.ct3SampleTimeComboBox->currentText().toInt();
	float angle = d_lineDetScanWidget->ui.ct3AngleLineEdit->text().toFloat();
}

void LineDetScanManager::updateControlsSlot()
{
	bool controlConnected = d_controller->getConnected();
	bool idle = d_controller->readIdleStatus();
	bool ready = d_controller->readReadyStatus();
	bool detConnected = d_lineDetNetWork->getConnected();
	bool readyToScan = controlConnected && idle && ready && detConnected;
	d_lineDetScanWidget->enableScan(readyToScan);
	QString tip;

	if (!controlConnected)
		tip += QString::fromLocal8Bit("控制系统未连接！\n");
	else
	{
		if (!ready)
			tip += QString::fromLocal8Bit("控制系统未初始化！\n");
		if (!idle)
			tip += QString::fromLocal8Bit("系统忙！\n");
	}

	if (!detConnected)
		tip += QString::fromLocal8Bit("探测器未连接！\n");

	d_lineDetScanWidget->setScanButtonToolTip(tip);
	emit readyToScanSignal(readyToScan);
}

void LineDetScanManager::stopButtonSlot()
{
	if (d_scan)
		d_scan->stopScan();

	d_controller->stopAll();
}

void LineDetScanManager::LoadCt3TemplateButtonSlot()
{
	d_ct3TemplateDialog = new CT3TemplateDialog(d_ct3Data, d_ct3TemplateDataItem, d_lineDetScanWidget);
	connect(d_ct3TemplateDialog, &CT3TemplateDialog::useItemSignal,
		this, &LineDetScanManager::useCt3ItemSlot);
	d_ct3TemplateDialog->show();
}

void LineDetScanManager::useCt3ItemSlot()
{
	d_lineDetScanWidget->useCt3Item(d_ct3TemplateDataItem);
}

void LineDetScanManager::bkgTuneSlot()
{
	std::unique_ptr<LineDetScanInterface> newAirDispose{ new LineDetBkgTune(d_controller, d_lineDetNetWork, 
		d_setupData, d_detNum) };
	d_newLineDetAirDisposeDialog = new AddModifyAirDisposeDialog(newAirDispose.get(), nullptr);
	d_newLineDetAirDisposeDialog->exec();
}

void LineDetScanManager::airTuneSlot()
{	
	std::unique_ptr<LineDetScanInterface> newAirDispose{ new LineDetAirTune(d_controller, d_lineDetNetWork,
		d_setupData, d_detNum) };
	d_newLineDetAirDisposeDialog = new AddModifyAirDisposeDialog(newAirDispose.get(), nullptr);
	d_newLineDetAirDisposeDialog->exec();
}

void LineDetScanManager::loadTuneDataSlot()
{
	d_airDisposeDialog = new LineDetAirDisposeDialog(d_orgDirectory, nullptr);
	//connect(this, &LineDetScanWidget::readyToScanSignal, d_airDisposeDialog, &LineDetAirDisposeDialog::updateScanButtonSlot);
	d_airDisposeDialog->exec();
}

void LineDetScanManager::getFileNameFromDialog(QString& _objectName, QString& _objectNumber, QString& _fileNumber, 
	QString& _comment)
{
	LineDetFileNameDialog* dlg = new LineDetFileNameDialog(_objectName, _objectNumber, _fileNumber,	_comment, nullptr);
	dlg->exec();
	delete dlg;
}

