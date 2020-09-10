#include "stdafx.h"
#include "conescanwidget.h"
#include "conescan.h"
#include "../PanelDll/panel.h"

ConeScanWidget::ConeScanWidget(MotorControlWidget* _motorControl, int _rayId, int _panelDetId, const std::vector<ScanMode>& _scanMode,
	SetupData* _setupData, Panel* _panel, ControllerInterface* _controller, QWidget *parent)
	: QWidget(parent), d_panel(_panel), d_controller(_controller), d_rayNum(_rayId), d_detNum(_panelDetId), d_setupData(_setupData)
{
	ui.setupUi(this);

	for (auto& scanMode : _scanMode)
	{
		if (scanMode == ScanMode::CONE_SCAN)
		{
			auto itr = std::find_if(d_setupData->coneScanData.begin(), d_setupData->coneScanData.end(),
				[=](ConeScanData& _Data)
			{	return _Data.Ray == _rayId && _Data.Det == _panelDetId; });

			if(itr != d_setupData->coneScanData.end())
				initiliseConeScanControls(*itr);
		}
		else if (scanMode == ScanMode::CONE_JOINT_ROT_SCAN)
		{
			auto itr = std::find_if(d_setupData->coneJointRotScanData.begin(), d_setupData->coneJointRotScanData.end(),
				[=](ConeJointRotScanData& _Data)
			{	return _Data.Ray == _rayId && _Data.Det == _panelDetId; });
			
			if (itr != d_setupData->coneJointRotScanData.end())
				initiliseConeJointRotScanControls(*itr);
		}
	}
}

ConeScanWidget::~ConeScanWidget()
{

}

void ConeScanWidget::setConeScanProgress(int _progress, const QString & _msg)
{
	ui.coneScanAllProgressBar->setValue(_progress);
	ui.coneScanProgressStaticLabel->setText(_msg);
}

template<typename T>
void addItemToMatixVieSample(T& _data, QComboBox* _matrix)
{
	for (auto& value : _data.Matrix)
		_matrix->addItem(QString::number(value));
}

void ConeScanWidget::initiliseConeScanControls(ConeScanData & _data)
{
	addItemToMatixVieSample(_data, ui.coneScanMatrixComboBox);
}

void ConeScanWidget::initiliseConeJointRotScanControls(ConeJointRotScanData & _data)
{
	addItemToMatixVieSample(_data, ui.coneJointRotScanMatrixComboBox);
}

void ConeScanWidget::on_scanProgressUpdated()
{
	int thisRound, allProgress;
	QString msg;
	d_scan->getScanProgress(thisRound, allProgress, msg);

	if (dynamic_cast<ConeScan*>(d_scan.get()))
		setConeScanProgress(allProgress, msg);

}

void ConeScanWidget::on_coneScanBeginSampleButton_clicked()
{
	d_scan.reset(new ConeScan(d_panel, d_controller, d_panelImageProcess.get()));
	d_scan->setDisposeFlag(ui.bkgTuneCheckBox->isChecked(), ui.airTuneCheckBox->isChecked(),
		ui.defectTuneCheckBox->isChecked(), true);
}
 