#include "stdafx.h"
#include "paneldetscanmanager.h"
#include "conescanwidget.h"
#include "conescan.h"
#include "controllerinterface.h"
#include "../PanelDll/panel.h"
#include "panelframeshot.h"
#include "ImageDialogManager.h"

PanelDetScanManager::PanelDetScanManager(int _rayId, int _panelDetId, const std::vector<ScanMode>& _scanMode
	, SetupData* _setupData, Panel* _panel, ControllerInterface* _controller, ImageDialogManager* _imageWidgetManager
	, QWidget *widgetParent, QObject *objectParent)
	: QObject(objectParent)
	, d_rayNum(_rayId), d_detNum(_panelDetId), d_panel(_panel), d_controller(_controller), d_imageWidgetManager(_imageWidgetManager)
	, d_setupData(_setupData), d_coneScanWidget(new ConeScanWidget(_panel, _panel->getWidget(), widgetParent)), d_imageDialogHandle(nullptr)
	, d_panelFrameShot(new PanelFrameShot(_panel, d_panelImageProcess.get(), nullptr))
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
	connect(d_coneScanWidget, &ConeScanWidget::frameShotSignal,	this, &PanelDetScanManager::frameShotSlot);
	connect(d_coneScanWidget, &ConeScanWidget::stopControllerSignal, this, &PanelDetScanManager::stopControllerSlot);
	connect(d_panelFrameShot.get(), &PanelFrameShot::imageAcquiredSignal, this, &PanelDetScanManager::showImageSlot);
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

void PanelDetScanManager::coneScanBeginSlot()
{	//TODO_DJ£ºresetµÄÊ±ºòÏß³Ì¹ÒÁË
	d_scan.reset(new ConeScan(d_panel, d_controller, d_panelImageProcess.get()));
	d_scan->setDisposeFlag(d_coneScanWidget->ui.bkgTuneCheckBox->isChecked(), 
		d_coneScanWidget->ui.airTuneCheckBox->isChecked(),
		d_coneScanWidget->ui.defectTuneCheckBox->isChecked(), true);

	//DEBUG£º²âÊÔÃæ°ådllµ¼³öwidgetÁÙÊ±×¢ÊÍ
	//int cycleTime = d_coneScanWidget->ui.cycleTimeEdit->text().toInt();
	//auto graduationTime = d_panel->caculateExTriggerSampleTime(cycleTime);
	//int graduation = d_coneScanWidget->ui.coneScanGraduationComboBox->currentText().toInt();
	//int framesPerGraduation = d_coneScanWidget->ui.coneScanframesComboBox->currentText().toInt();
	//float oriencInc = d_coneScanWidget->ui.orientIncEdit->text().toFloat();
	//unsigned short gainFactor = d_coneScanWidget->getGainFactor(d_coneScanWidget->ui.gainComboBox->currentText());
	//connect(d_scan.get(), &ConeScanInterface::scanProgressSignal, this, &PanelDetScanManager::scanProgressSlot);
	//d_scan->beginScan(graduation, framesPerGraduation, graduationTime, cycleTime, gainFactor, oriencInc);
	//DEBUG
}

void PanelDetScanManager::scanProgressUpdatedSlot()
{
	int thisRound, allProgress;
	QString msg;
	d_scan->getScanProgress(thisRound, allProgress, msg);

	if (dynamic_cast<ConeScan*>(d_scan.get()))
		d_coneScanWidget->setConeScanProgress(allProgress, msg);
}

void PanelDetScanManager::frameShotSlot()
{
	//DEBUG£º²âÊÔÃæ°ådllµ¼³öwidgetÁÙÊ±×¢ÊÍ
	//auto frames = d_coneScanWidget->ui.singleShotFramesSpinBox->text().toInt();
	//auto cycleTime = d_coneScanWidget->ui.cycleTimeEdit->text().toInt();
	//unsigned short gainFactor = d_coneScanWidget->getGainFactor(d_coneScanWidget->ui.gainComboBox->currentText());
	//d_panelFrameShot->beginAcquire(frames, cycleTime, gainFactor);
	//DEBUG£º²âÊÔÃæ°ådllµ¼³öwidgetÁÙÊ±×¢ÊÍ
	//DEBUG£º²âÊÔÍ¼ÏñËõ·Å×¢ÊÍ
	//d_imageDialogHandle = d_imageWidgetManager->getNewWindow(
	//	d_panel->getPanelSize().first, d_panel->getPanelSize().second);
	//DEBUG£º²âÊÔÍ¼ÏñËõ·Å×¢ÊÍ

	//DEBUG£º²âÊÔÍ¼ÏñËõ·ÅÐÂÔö
	d_imageDialogHandle = d_imageWidgetManager->getNewWindow(2048, 2048);
	//DEBUG£º²âÊÔÍ¼ÏñËõ·ÅÐÂÔö
}

void PanelDetScanManager::stopControllerSlot()
{
	d_controller->stopAll();
}

void PanelDetScanManager::scanProgressSlot(float _progress)
{
	d_coneScanWidget->setConeScanProgress(_progress, QString(""));
}

void PanelDetScanManager::showImageSlot(unsigned short* _image, int _width, int _height)
{
	//DEBUG£º²âÊÔÍ¼ÏñËõ·ÅÐÂÔö
	d_imageDialogHandle = d_imageWidgetManager->getNewWindow(2048, 2048);
	//DEBUG£º²âÊÔÍ¼ÏñËõ·ÅÐÂÔö
	d_imageWidgetManager->showImageInCurrentWindow(
		d_imageDialogHandle, (unsigned char*)_image, _width, _height);
}
