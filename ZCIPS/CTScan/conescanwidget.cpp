#include "stdafx.h"
#include "conescanwidget.h"
#include "../PanelDll/panel.h"

ConeScanWidget::ConeScanWidget(Panel* _panel, QWidget *parent)
	: QWidget(parent)
	, d_panel(_panel)
{
	ui.setupUi(this);
	ui.singleShotFramesSpinBox->setValue(1);
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


void ConeScanWidget::on_coneScanBeginSampleButton_clicked()
{
	int time = ui.cycleTimeEdit->text().toInt();
	d_panel->setSampleMode(SampleMode::exTrigger, time);
	emit coneScanBeginSignal();
}
 
void ConeScanWidget::on_frameShotButton_clicked()
{
	emit frameShotSignal();
}

void ConeScanWidget::on_gainGroupBox_currentIndexChanged(int _index)
{
	d_panel->setGainFactor(_index);
}

void ConeScanWidget::on_cycleTimeEdit_returnPressed()
{
	int time = ui.cycleTimeEdit->text().toInt();
	d_panel->setCycleTime(time);
}

