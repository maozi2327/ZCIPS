#include "stdafx.h"
#include "paneldetscanmanager.h"
#include "conescanwidget.h"
#include "conescan.h"
#include "../PanelDll/panel.h"
#include "panelframeshot.h"

PanelDetScanManager::PanelDetScanManager(int _rayId, int _panelDetId, const std::vector<ScanMode>& _scanMode
	, SetupData* _setupData, Panel* _panel, ControllerInterface* _controller
	, QWidget *widgetParent, QObject *objectParent)
	: QObject(objectParent)
	, d_rayNum(_rayId), d_detNum(_panelDetId), d_panel(_panel), d_controller(_controller)
	, d_setupData(_setupData), d_coneScanWidget(new ConeScanWidget(_panel, widgetParent))
	, d_panelFrameShot(new PanelFrameShot(_panel, nullptr))
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

	connect(d_coneScanWidget, &ConeScanWidget::coneScanBeginSignal,
		this, &PanelDetScanManager::coneScanBeginSlot);
	connect(d_coneScanWidget, &ConeScanWidget::frameShotSignal,
		this, &PanelDetScanManager::frameShotSlot);
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
{
	d_scan.reset(new ConeScan(d_panel, d_controller, d_panelImageProcess.get()));
	d_scan->setDisposeFlag(d_coneScanWidget->ui.bkgTuneCheckBox->isChecked(), 
		d_coneScanWidget->ui.airTuneCheckBox->isChecked(),
		d_coneScanWidget->ui.defectTuneCheckBox->isChecked(), true);
	d_scan->sendCmdToController();
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
	auto frames = d_coneScanWidget->ui.singleShotFramesSpinBox->text().toInt();
	auto cycleTime = d_coneScanWidget->ui.cycleTimeEdit->text().toInt();
	d_panelFrameShot->beginAcquire(frames, cycleTime);
}