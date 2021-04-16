#include "stdafx.h"
#include "paneldetscanmanager.h"
#include "conescanwidget.h"
#include "conescan.h"
#include "controllerinterface.h"
#include "../PanelDetector/panel.h"
#include "../Public/util/messagebox.h"
#include "panelframeshot.h"
#include "ImageDialogManager.h"
#include "linedetfilenamedialog.h"
#include "newpaneldetbkgtunedialog.h"
#include "panelbkgtune.h"
#include "newpaneldetairtunedialog.h"
#include "panelairtune.h"
#include "loadtunedatadialog.h"
#include "conejointscan.h"

PanelDetScanManager::PanelDetScanManager(int _rayId, int _panelDetId, const std::vector<ScanMode>& _scanMode, 
	SetupData* _setupData, Panel* _panel, ControllerInterface* _controller, ImageDialogManager* _imageWidgetManager,
	const QString& _tunedDirectory, const QString& _orgDirectory, const QString& _tunedBkgDirectory,
	const QString& _tunedAirDirectory, const QString& _tunedConeJointAirDirectory, const QString& _defectFileName, QWidget *widgetParent, QObject *objectParent)
	: QObject(objectParent)
	, d_rayNum(_rayId), d_detNum(_panelDetId), d_panel(_panel), d_controller(_controller), d_imageWidgetManager(_imageWidgetManager)
	, d_setupData(_setupData), d_coneScanWidget(new ConeScanWidget(_panel, _panel->getWidget(), widgetParent)), d_imageDialogHandle(nullptr)
	, d_panelImageProcess(new PanelImageProcess(_panel->getPanelSize().first, _panel->getPanelSize().second))
	, d_orgDirectory(_orgDirectory), d_tunedFileDirectory(_tunedDirectory), d_tunedBkgDirectory(_tunedBkgDirectory), d_tunedAirDirectory(_tunedAirDirectory)
	, d_tunedConeJointAirDirectory(_tunedConeJointAirDirectory), d_defectFileName(_defectFileName)
{
	for (auto& scanMode : _scanMode)
	{
		if (scanMode == ScanMode::CONE_SCAN)
		{
			auto itr = std::find_if(d_setupData->coneScanData.begin(), d_setupData->coneScanData.end(),
				[=](ConeScanData& _Data)
			{	return _Data.Ray == _rayId && _Data.Det == _panelDetId; });

			if (itr != d_setupData->coneScanData.end())
				d_coneScanWidget->initiliseConeScanControls(*itr);
		}
		else if (scanMode == ScanMode::CONE_JOINT_ROT_SCAN)
		{
			auto itr = std::find_if(d_setupData->coneJointRotScanData.begin(), d_setupData->coneJointRotScanData.end(),
				[=](ConeJointRotScanData& _Data)
			{	return _Data.Ray == _rayId && _Data.Det == _panelDetId; });

			if (itr != d_setupData->coneJointRotScanData.end())
				d_coneScanWidget->initiliseConeJointRotScanControls(*itr);
		}
	}

	connect(d_coneScanWidget, &ConeScanWidget::coneScanBeginSignal,	this, &PanelDetScanManager::coneScanBeginSlot);
	connect(d_coneScanWidget, &ConeScanWidget::saveConeScanConfigSignal, this, &PanelDetScanManager::saveConeScanConfigSlot);
	connect(d_coneScanWidget, &ConeScanWidget::coneJointScanBeginSignal, this, &PanelDetScanManager::coneJointScanBeginSlot);
	connect(d_coneScanWidget, &ConeScanWidget::frameShotSignal,	this, &PanelDetScanManager::frameShotSlot);
	connect(d_coneScanWidget, &ConeScanWidget::coneScanStopSignal, this, &PanelDetScanManager::coneScanStopSlot);
	connect(d_coneScanWidget, &ConeScanWidget::previewSignal, this, &PanelDetScanManager::previewSlot);
	connect(d_coneScanWidget, &ConeScanWidget::bkgTuneSignal, this, &PanelDetScanManager::bkgTuneSlot);
	connect(d_coneScanWidget, &ConeScanWidget::airTuneSignal, this, &PanelDetScanManager::airTuneSlot);
	connect(d_coneScanWidget, &ConeScanWidget::loadTuneDataSignal, this, &PanelDetScanManager::loadTuneDataSlot);
	connect(d_coneScanWidget, &ConeScanWidget::loadConeJointTuneDataSignal, this, &PanelDetScanManager::loadConeJointScanTuneDataSlot);
	
	d_panelImageProcess->loadDefectData(d_defectFileName);
}

PanelDetScanManager::~PanelDetScanManager()
{

}

void PanelDetScanManager::frameShotCallback(unsigned short * _image)
{

}

QWidget * PanelDetScanManager::getWidget()
{
	return d_coneScanWidget;
}

void PanelDetScanManager::updateFileDirectory(const QString& _orgPath, const QString& _tunedFileDirectory)
{
	d_orgDirectory = _orgPath;
	d_tunedFileDirectory = _tunedFileDirectory;
}

void PanelDetScanManager::coneScanBeginSlot()
{
	d_panel->stopAcquire();
	bool correctFlag = d_coneScanWidget->ui.allTuneCheckBox->isChecked();

	if (correctFlag && !d_loadTunedDataFlag)
	{
		messageBox(QString::fromLocal8Bit("´íÎó"), QString::fromLocal8Bit("Î´¼ÓÔØÐ£ÕýÊý¾Ý"));
		return;
	}

	QString fileNumber{ QString::fromLocal8Bit("55AA5A") };

	if (!getFileNameFromDialog(d_objectName, d_objectNumber, fileNumber, d_fileNameComment))
		return;

	QString orgFileName = d_orgDirectory + QDateTime::currentDateTime().date().toString(Qt::ISODate) + '/' +
		d_objectName + '/' + d_objectNumber + '/' + fileNumber + QString::fromLocal8Bit("_") + d_fileNameComment;
	QString destFilePath = d_tunedFileDirectory + QDateTime::currentDateTime().date().toString(Qt::ISODate) + '/' +
		d_objectName + '/' + d_objectNumber + '/';
	PanDetData panDetData = d_setupData->panDetData[d_detNum];
	d_scan.reset(new ConeScan(d_panel, d_controller, d_panelImageProcess.get(), correctFlag, correctFlag, correctFlag, panDetData));
	d_scan->setFileName(orgFileName, destFilePath);
	int cycleTime = d_panel->getSampleTimeSet();
	auto graduationTime = d_panel->caculateExTriggerSampleTime(cycleTime);
	int graduation = d_coneScanWidget->ui.coneScanGraduationComboBox->currentText().toInt();
	int framesPerGraduation = d_coneScanWidget->ui.coneScanframesComboBox->currentText().toInt();
	float oriencInc = d_coneScanWidget->ui.coneScanOrientIncEdit->text().toFloat();
	auto gainFactor = d_panel->getGainFactorSet();
	float slicePos = d_coneScanWidget->ui.slicePosEdit->text().toFloat();
	connect(d_scan.get(), &ConeScanInterface::scanProgressSignal, this, &PanelDetScanManager::scanProgressSlot);
	d_scan->beginScan(graduation, framesPerGraduation, 1, graduationTime, cycleTime, gainFactor, oriencInc, slicePos, 
		d_controller->readAxisPostion(Axis::objRadial), d_controller->readAxisPostion(Axis::detRadial));
}

//void PanelDetScanManager::scanProgressUpdatedSlot()
//{
//	int thisRound, allProgress;
//	QString msg;
//	d_scan->getScanProgress(thisRound, allProgress, msg);
//
//	if (dynamic_cast<ConeScan*>(d_scan.get()))
//		d_coneScanWidget->setConeScanProgress(allProgress, msg);
//}
//TODO_DJ£ºÎÄ¼þ´æÔÚfunction.h messageBox
//TODO_DJ£º¼ÓÔØ±³¾°Êý¾Ý
void PanelDetScanManager::coneJointScanBeginSlot()
{
	d_panel->stopAcquire();
	bool correctFlag = d_coneScanWidget->ui.allTuneCheckBox->isChecked();
	QString fileNumber{ QString::fromLocal8Bit("55AA5A") };

	if (correctFlag && !d_loadConeJointTunedDataFlag)
	{
		messageBox(QString::fromLocal8Bit("´íÎó"), QString::fromLocal8Bit("Î´¼ÓÔØÐ£ÕýÊý¾Ý"));
		return;
	}

	if (!getFileNameFromDialog(d_objectName, d_objectNumber, fileNumber, d_fileNameComment))
		return;

	QString orgFileName = d_orgDirectory + QDateTime::currentDateTime().date().toString(Qt::ISODate) + '/' +
		d_objectName + '/' + d_objectNumber + '/' + fileNumber + QString::fromLocal8Bit("_") + d_fileNameComment;
	QString destFilePath = d_tunedFileDirectory + QDateTime::currentDateTime().date().toString(Qt::ISODate) + '/' +
		d_objectName + '/' + d_objectNumber + '/';
	PanDetData panDetData = d_setupData->panDetData[d_detNum];
	d_scan.reset(new ConeJointScan(d_panel, d_controller, d_panelImageProcess.get(), correctFlag, correctFlag, correctFlag, 
		panDetData, d_airFileNameA, d_airFileNameB));
	d_scan->setFileName(orgFileName, destFilePath);
	int cycleTime = d_panel->getSampleTimeSet();
	auto graduationTime = d_panel->caculateExTriggerSampleTime(cycleTime);
	int graduation = d_coneScanWidget->ui.coneJointGraduationComboBox->currentText().toInt();
	int framesPerGraduation = d_coneScanWidget->ui.coneJointFramesComboBox->currentText().toInt();
	float oriencInc = d_coneScanWidget->ui.coneJointOrientIncEdit->text().toFloat();
	auto gainFactor = d_panel->getGainFactorSet();
	float slicePos = d_coneScanWidget->ui.coneJointSlicePosEdit->text().toFloat();
	connect(d_scan.get(), &ConeScanInterface::scanProgressSignal, this, &PanelDetScanManager::scanProgressSlot);
	static_cast<ConeJointScan*>(d_scan.get())->beginScan(graduation, framesPerGraduation, 2, graduationTime, cycleTime, gainFactor, oriencInc, slicePos,
		d_controller->readAxisPostion(Axis::objRadial), d_controller->readAxisPostion(Axis::detRadial), -50, 100);
}

struct ConeScanDataRawItem
{
	QByteArray byteArrayData;
	ConeScanDataRawItem(const coneScanDataItem& item)
	{
		QByteArray itemNameByte = item.itemName.toLocal8Bit();
		QByteArray airNameByte = item.airName.toLocal8Bit();
		int itemNameLenth = itemNameByte.size();
		int airNameLenth = airNameByte.size();
		byteArrayData += QByteArray(sizeof(int), 0);
		byteArrayData += QByteArray::fromRawData((char*)(&itemNameLenth), sizeof(int));
		byteArrayData += QByteArray::fromRawData(itemNameByte.data(), itemNameLenth);
		byteArrayData += QByteArray::fromRawData((char*)(&item.rayIndex), sizeof(int));
		byteArrayData += QByteArray::fromRawData((char*)(&item.paneldetIndex), sizeof(int));
		byteArrayData += QByteArray::fromRawData((char*)(&item.graduationComboxIndex), sizeof(int));
		byteArrayData += QByteArray::fromRawData((char*)(&item.frameComboxIndex), sizeof(int));
		byteArrayData += QByteArray::fromRawData((char*)(&item.orient), sizeof(float));
		byteArrayData += QByteArray::fromRawData((char*)(&item.sod), sizeof(float));
		byteArrayData += QByteArray::fromRawData((char*)(&item.sdd), sizeof(float));
		byteArrayData += QByteArray::fromRawData((char*)(&airNameLenth), sizeof(int));
		byteArrayData += QByteArray::fromRawData(airNameByte.data(), sizeof(int));
		byteArrayData += QByteArray::fromRawData(0, 10 * sizeof(float));
		byteArrayData += QByteArray::fromRawData((char*)(&item.panelData.gainFactorIndex), sizeof(int));
		byteArrayData += QByteArray::fromRawData((char*)(&item.panelData.cycleTime), sizeof(int));
		byteArrayData += QByteArray::fromRawData(0, 10 * sizeof(float));
	}

	char* getRawData(int& lenth)
	{
		lenth = byteArrayData.size();
		return byteArrayData.data();
	}
	//int currentItemLenth;
	//int itemNameLenth;
	///*char* itemName;*/
	//int rayIndex;
	//int paneldetIndex;
	//int graduationComboxIndex;
	//int frameComboxIndex;
	//float orient;
	//float sod;
	//float sdd;
	//int airNameLenth;
	///*char* airName; */
	//float re[10];
	//int gainFactorIndex;
	//float cycleTime;
	//float re[10];
};

char* charSeek(char* pos, int lenth)
{
	return pos + lenth;
}

void PanelDetScanManager::saveConeScanConfigSlot()
{
	coneScanDataItem item;
	SaveConeScanConfigDialog dialog(d_rayNum, d_detNum, d_coneScanWidget->ui.slicePosEdit->text().toFloat(),
		d_coneScanWidget->ui.coneScanOrientIncEdit->text().toFloat(), d_controller->readAxisPostion(Axis::objRadial), 
		d_controller->readAxisPostion(Axis::detRadial), d_panel, &item, d_coneScanWidget->ui.coneScanGraduationComboBox,
		d_coneScanWidget->ui.coneScanframesComboBox, d_airName, d_tunedAirDirectory);

	if (dialog.exec() == QDialog::Accepted)
	{
		d_coneScanConfigFileName = QString::fromLocal8Bit("d:\\config.dat");
		QFile configFile(d_coneScanConfigFileName);
		configFile.open(QFile::ReadWrite);
		QByteArray byteArray = configFile.readAll();
		char* fileRawData = byteArray.data();
		*(int*)(fileRawData + 4) = *(int*)(fileRawData + 4) + 1;
		int itemLenth;
		char* rawData = ConeScanDataRawItem(item).getRawData(itemLenth);
		configFile.write(rawData, itemLenth);
		configFile.close();
	}
}

void PanelDetScanManager::frameShotSlot()
{
	//DEBUG£º²âÊÔÃæ°ådllµ¼³öwidgetÁÙÊ±×¢ÊÍ
	int frames = 1;
	auto cycleTime = d_panel->getSampleTimeSet();
	auto gainFactor = d_panel->getGainFactorSet();
	QString fileNumber{ QString::fromLocal8Bit("55AA5A") };
	
	if(!getFileNameFromDialog(d_objectName, d_objectNumber, fileNumber, d_fileNameComment))
		return;

	QString orgFileName = d_orgDirectory + QDateTime::currentDateTime().date().toString(Qt::ISODate) + '/' +
		d_objectName + '/' + d_objectNumber + '/' + fileNumber + QString::fromLocal8Bit("_") + d_fileNameComment + QString::fromLocal8Bit(".dcm");
	QString destFilePath = d_tunedFileDirectory + QDateTime::currentDateTime().date().toString(Qt::ISODate) + '/' +
		d_objectName + '/' + d_objectNumber + '/';
	bool correctFlag = d_coneScanWidget->ui.allTuneCheckBox->isChecked();
	d_panelFrameShot.reset(new PanelFrameShot(d_panel, d_panelImageProcess.get(), correctFlag, correctFlag, correctFlag, nullptr));
	d_panelFrameShot->setFileName(orgFileName, destFilePath);
	d_panelFrameShot->setDicomInfo(d_objectName, d_objectNumber);
	connect(d_panelFrameShot.get(), &PanelFrameShot::imageAcquiredSignal, this, &PanelDetScanManager::showImageSlot);
	connect(d_panelFrameShot.get(), &PanelFrameShot::frameAcquiredSignal, this, &PanelDetScanManager::scanProgressSlot);
	d_panelFrameShot->beginAcquire(frames, cycleTime, gainFactor);
	//DEBUG£º²âÊÔÃæ°ådllµ¼³öwidgetÁÙÊ±×¢ÊÍ
	//DEBUG£º²âÊÔÍ¼ÏñËõ·Å×¢ÊÍ
	//d_imageDialogHandle = d_imageWidgetManager->getNewWindow(
	//	d_panel->getPanelSize().first, d_panel->getPanelSize().second);
	//DEBUG£º²âÊÔÍ¼ÏñËõ·Å×¢ÊÍ

	//DEBUG£º²âÊÔÍ¼ÏñËõ·ÅÐÂÔö
	//d_imageDialogHandle = d_imageWidgetManager->getNewWindow(2048, 2048);
	//DEBUG£º²âÊÔÍ¼ÏñËõ·ÅÐÂÔö
	//d_panelFrameShot->beginAcquire(0, 0, 0);
}

void PanelDetScanManager::coneScanStopSlot()
{
	d_controller->stopAll();
	d_panel->stopAcquire();
	
	if (d_scan)
		d_scan->stopScan();

	if (d_panelFrameShot)
		d_panelFrameShot->stopAcquire();
}

void PanelDetScanManager::previewSlot()
{
	auto cycleTime = d_panel->getSampleTimeSet();
	auto gainFactor = d_panel->getGainFactorSet();
	d_panelFrameShot->beginPreview(cycleTime, gainFactor);
}

void PanelDetScanManager::bkgTuneSlot()
{
	PanelBkgTune panelBkgTune(d_panel, d_panelImageProcess.get());
	NewPanelDetBkgTuneDialog dialog(d_orgDirectory, d_tunedBkgDirectory, 
		&panelBkgTune, d_panel->getSampleTimeSet(), d_panel->getGainFactorSet(), nullptr);
	dialog.exec();
}

void PanelDetScanManager::airTuneSlot()
{
	PanelAirTune panelAirTune(d_panel, d_panelImageProcess.get());
	NewPanelDetAirTuneDialog dialog(d_tunedBkgDirectory, d_orgDirectory, d_tunedAirDirectory,
		&panelAirTune, d_panel->getSampleTimeSet(), d_panel->getGainFactorSet(), nullptr);
	dialog.exec();
}

void PanelDetScanManager::loadTuneDataSlot()
{
	QString airName;
	LoadTuneDataDialog loadTuneDataDialog(d_tunedAirDirectory, airName);

	if (loadTuneDataDialog.exec() == QDialog::Accepted)
	{
		QString bkgName = d_tunedBkgDirectory +
			airName.right(airName.size() - (airName.indexOf(QString::fromLocal8Bit("_55AA5A_")) + QString::fromLocal8Bit("_55AA5A_").size()));
		d_airName = airName;
		d_bkgName = bkgName;
		d_panelImageProcess->loadBkgData(bkgName);
		d_panelImageProcess->loadAirData(airName);
		d_loadTunedDataFlag = true;
	}
}

void PanelDetScanManager::loadConeJointScanTuneDataSlot()
{
	QString airName, bkgName;
	LoadTuneDataDialog loadTuneDataDialog(d_tunedConeJointAirDirectory, airName);

	if (loadTuneDataDialog.exec() == QDialog::Accepted)
	{
		QString bkgName = d_tunedBkgDirectory +
			airName.right(airName.size() - (airName.indexOf(QString::fromLocal8Bit("_55AA5A_")) + QString::fromLocal8Bit("_55AA5A_").size()));
		d_panelImageProcess->loadBkgData(bkgName);
		QString namePerfix = airName.left(airName.lastIndexOf("_") + 1);
		d_airFileNameA = namePerfix + QString::fromLocal8Bit("0.tif");
		d_airFileNameB = namePerfix + QString::fromLocal8Bit("1.tif");
		d_loadConeJointTunedDataFlag = true;
	}
}

void PanelDetScanManager::scanProgressSlot(int _framesAcquiredThisRound, int _framesThisRound, int _framesAcquiredAll, int _framesALL, QString _message)
{
	d_coneScanWidget->setConeScanProgress(100 * _framesAcquiredThisRound / _framesThisRound, 100 * _framesAcquiredAll / _framesALL, _message);
}


void PanelDetScanManager::showImageSlot(unsigned short* _image, int _width, int _height)
{
	//DEBUG£º²âÊÔÍ¼ÏñËõ·ÅÐÂÔö
	//d_imageDialogHandle = d_imageWidgetManager->getNewWindow(2048, 2048);
	//DEBUG£º²âÊÔÍ¼ÏñËõ·ÅÐÂÔö
	d_imageWidgetManager->showImageInCurrentWindow(
		d_imageDialogHandle, (unsigned char*)_image, _width, _height);
}

void PanelDetScanManager::updateObjectNameNumber(const QString & _objectName, const QString & _objectNumber)
{
	d_objectName = _objectName;
	d_objectNumber = _objectNumber;
}

void PanelDetScanManager::updateControlSystemStatus(bool readyToScan)
{
	d_coneScanWidget->ui.coneScanBeginSampleButton->setEnabled(readyToScan);
	d_coneScanWidget->ui.airTuneButton->setEnabled(readyToScan);
	d_coneScanWidget->ui.bkgTuneButton->setEnabled(readyToScan);
}

void PanelDetScanManager::updatePanelStatus(bool readyToScan)
{
	d_coneScanWidget->ui.previewButton->setEnabled(readyToScan);
	d_coneScanWidget->ui.frameShotButton->setEnabled(readyToScan);
	d_coneScanWidget->ui.previewButton->setEnabled(readyToScan);
}

bool PanelDetScanManager::getFileNameFromDialog(QString& _objectName, QString& _objectNumber, 
	QString& _fileNumber, QString& _comment)
{
	LineDetFileNameDialog dlg(_objectName, _objectNumber, _fileNumber, _comment, nullptr);
	
	if(dlg.exec() == QDialog::Accepted)
		return true;

	return false;
}
