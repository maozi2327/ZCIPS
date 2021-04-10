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
#include "linedetairtunedialog.h"
#include "scantemplate.h"
#include "linedetfilenamedialog.h"
#include "newlinedetbkgtunedialog.h"

QWidget * LineDetScanManager::getWidget()
{
	return d_lineDetScanWidget;
}

LineDetScanManager::LineDetScanManager(int _rayId, int _lineDetId, const std::vector<ScanMode>& _scanMode, 
	SetupData* _setupData, LineDetNetWork* _lineDetNetWork, ControllerInterface* _controller, LineDetImageProcess* _lineDetImageProcess,
	const QString& _tunedDirectory, const QString& _orgDirectory, /*const QString& _objectName, 
	const QString& _objectNumber, */QWidget* _widgetParent, QObject *_ObjectParent)
	: QObject(_ObjectParent)
	, d_rayNum(_rayId), d_detNum(_lineDetId)
	, d_setupData(_setupData), d_lineDetNetWork(_lineDetNetWork), d_controller(_controller), d_lineDetImageProcess(_lineDetImageProcess)
	, d_orgDirectory(_orgDirectory), d_tunedFileDirectory(_tunedDirectory)
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
	connect(d_lineDetScanWidget, &LineDetScanWidget::bkgTuneSignal,
		this, &LineDetScanManager::bkgTuneSlot);
	connect(d_lineDetScanWidget, &LineDetScanWidget::loadTuneDataSignal,
		this, &LineDetScanManager::loadTuneDataSlot);
	connect(d_lineDetScanWidget, &LineDetScanWidget::LoadCt3TemplateButtonSignal, 
		this, &LineDetScanManager::LoadCt3TemplateButtonSlot);
	//d_timer = new QTimer();
	//d_timer->start(100);
}

LineDetScanManager::~LineDetScanManager()
{
}

void LineDetScanManager::updateFileDirectory(const QString& _orgPath, const QString& _tunedFileDirectory)
{
	d_orgDirectory = _orgPath;
	d_tunedFileDirectory = _tunedFileDirectory;
}

void LineDetScanManager::updateStatus(bool readyToScan)
{
	d_lineDetScanWidget->enableScan(readyToScan);
	emit readyToScanSignal(readyToScan);
}

void LineDetScanManager::updateObjectNameNumber(const QString & _objectName, const QString & _objectNumber)
{
	d_objectName = _objectName;
	d_objectNumber = _objectNumber;
}

void LineDetScanManager::ct3ScanSlot()
{
	auto layer = d_lineDetScanWidget->getLayer();
	int matrix = d_lineDetScanWidget->ui.ct3MatrixComboBox->currentText().toInt();
	int view = d_lineDetScanWidget->ui.ct3ViewComboBox->currentText().toInt();
	int sampleTime = d_lineDetScanWidget->ui.ct3SampleTimeComboBox->currentText().toInt();
	float angle = d_lineDetScanWidget->ui.ct3AngleLineEdit->text().toFloat();
	d_scan.reset(new CT3Scan(d_controller, d_lineDetNetWork, d_setupData, d_detNum, d_lineDetImageProcess));

	if (!dynamic_cast<CT3Scan*>(d_scan.get())->setScanParameter(layer.first, matrix, view, sampleTime, angle, layer.second))
		return;

	QString fileNumber{QString::fromLocal8Bit("55AA5A")};

	if(!getFileNameFromDialog(d_objectName, d_objectNumber, fileNumber, d_fileNameComment))
		return;

	QString orgFileName = d_orgDirectory + QDateTime::currentDateTime().date().toString(Qt::ISODate)  + '/' + 
		d_objectName + '/' + d_objectNumber + '/' + fileNumber + QString::fromLocal8Bit("_") + d_fileNameComment + QString::fromLocal8Bit(".org");
	QString destFilePath = d_tunedFileDirectory + QDateTime::currentDateTime().date().toString(Qt::ISODate) + '/' + 
		d_objectName + '/' + d_objectNumber + '/';
	d_scan->setFileName(orgFileName, destFilePath);

	connect(d_scan.get(), &CT3Scan::samplePercentCountSignal, d_lineDetScanWidget,
		&LineDetScanWidget::updateCT3ProgresserSlot);
	d_scan->beginScan();
}

void LineDetScanManager::drScanSlot()
{
	QString objectName, objectNumber, fileNumber, comment;
	
	if(!getFileNameFromDialog(objectNumber, objectNumber, fileNumber, comment))
		return;
	
	int matrix = d_lineDetScanWidget->ui.ct3MatrixComboBox->currentText().toInt();
	int view = d_lineDetScanWidget->ui.ct3ViewComboBox->currentText().toInt();
	int sampleTime = d_lineDetScanWidget->ui.ct3SampleTimeComboBox->currentText().toInt();
	float angle = d_lineDetScanWidget->ui.ct3AngleLineEdit->text().toFloat();
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
	LineDetBkgTune newBkgTune(d_controller, d_lineDetNetWork, d_setupData, d_detNum, d_lineDetImageProcess);
	d_newLineDetBkgTuneDialog = new NewLineDetBkgTuneDialog(d_orgDirectory, d_tunedFileDirectory,
		&newBkgTune, nullptr);
	d_newLineDetBkgTuneDialog->exec();
}

void LineDetScanManager::airTuneSlot()
{	
	std::unique_ptr<LineDetScanInterface> newAirTune{ new LineDetAirTune(d_controller, d_lineDetNetWork,
		d_setupData, d_detNum, d_lineDetImageProcess) };
	d_newLineDetAirTuneDialog = new NewLineDetAirTuneDialog(d_orgDirectory, d_tunedFileDirectory,
		newAirTune.get(), nullptr);
	d_newLineDetAirTuneDialog->exec();
}

void LineDetScanManager::loadTuneDataSlot()
{
	d_airTuneDialog = new LineDetAirTuneDialog(d_orgDirectory, nullptr);
	d_airTuneDialog->exec();
}

bool LineDetScanManager::getFileNameFromDialog(QString& _objectName, QString& _objectNumber, QString& _fileNumber, 
	QString& _comment)
{
	LineDetFileNameDialog dlg(_objectName, _objectNumber, _fileNumber,	_comment, nullptr);
	
	if(dlg.exec() == QDialog::Accepted)
		return true;

	return false;
}

