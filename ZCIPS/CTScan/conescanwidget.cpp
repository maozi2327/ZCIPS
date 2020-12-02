#include "stdafx.h"
#include "conescanwidget.h"
#include "../PanelDll/panel.h"
#include "qtwidgetaddtest.h"

ConeScanWidget::ConeScanWidget(Panel* _panel, QWidget* _panelWidget, QWidget *parent)
	: QWidget(parent)
	, d_panel(_panel), d_panelWidget(_panelWidget)
{
	ui.setupUi(this);
	ui.coneScanGraduationComboBox->setCurrentText("256");
	ui.coneScanframesComboBox->setCurrentText("1");
	ui.orientIncEdit->setText("0");
}

ConeScanWidget::~ConeScanWidget()
{

}

void ConeScanWidget::setPanelDetWidget()
{
	d_panelWidget->setParent(ui.widget);
}

void ConeScanWidget::setConeScanProgress(float _progress, const QString & _msg)
{
	ui.coneScanAllProgressBar->setValue(_progress * 100);
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
	addItemToMatixVieSample(_data, ui.coneScanGraduationComboBox);
}

void ConeScanWidget::initiliseConeJointRotScanControls(ConeJointRotScanData & _data)
{
	addItemToMatixVieSample(_data, ui.coneJointRotScanMatrixComboBox);
}

unsigned short ConeScanWidget::getGainFactor(const QString & _text)
{
	unsigned short gain = 2;

	if (_text == "0.25pF")
		gain = 0;
	else if (_text == "0.5pF")
		gain = 1;
	else if (_text == "1pF")
		gain = 2;
	else if (_text == "2pF")
		gain = 3;
	else if (_text == "4pF")
		gain = 4;
	else if (_text == "8pF")
		gain = 5;

	return gain;
}

void ConeScanWidget::on_coneScanBeginSampleButton_clicked()
{
	emit coneScanBeginSignal();
}
 
void ConeScanWidget::on_frameShotButton_clicked()
{
	emit frameShotSignal();
}

void ConeScanWidget::on_coneScanStopButton_clicked()
{
	emit stopControllerSignal();
}
