#include "stdafx.h"
#include "conescanwidget.h"
#include "qtwidgetaddtest.h"

ConeScanWidget::ConeScanWidget(Panel* _panel, QWidget* _panelWidget, QWidget *parent)
	: QWidget(parent)
	, d_panel(_panel), d_panelWidget(_panelWidget)
{
	ui.setupUi(this);
	ui.panelWidget->setLayout(d_panelWidget->layout());
	ui.coneScanGraduationComboBox->setCurrentText("256");
	ui.coneScanOrientIncEdit->setText("0");
	//d_initialHeight = minimumHeight();
	//d_panelWidget->setParent(ui.panelWidget);
	//ui.panelWidget->setFixedSize(d_panelWidget->width(), d_panelWidget->height());
	//setFixedHeight(ui.panelWidget->height() + height());
	ui.airTuneCheckBox->setVisible(false);
	ui.bkgTuneCheckBox->setVisible(false);
	ui.defectTuneCheckBox->setVisible(false);
}

ConeScanWidget::~ConeScanWidget()
{

}

void ConeScanWidget::setConeScanProgress(int _progress, int _progressAll, const QString & _msg)
{
	ui.coneScanProgressBar->setValue(_progress);
	ui.coneScanAllProgressBar->setValue(_progressAll);
	ui.scanMessageLabel->setText(_msg);
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
	if (ui.tabWidget->currentWidget() == ui.coneScanTab)
		emit coneScanBeginSignal();
	else if (ui.tabWidget->currentWidget() == ui.coneJointScanTab)
		emit coneJointScanBeginSignal();
}
 
void ConeScanWidget::on_frameShotButton_clicked()
{
	emit frameShotSignal();
}

void ConeScanWidget::on_coneScanStopButton_clicked()
{
	emit coneScanStopSignal();
}

void ConeScanWidget::on_previewButton_clicked()
{
	emit previewSignal();
}

void ConeScanWidget::on_bkgTuneButton_clicked()
{
	emit bkgTuneSignal();
}

void ConeScanWidget::on_airTuneButton_clicked()
{
	emit airTuneSignal();
}

void ConeScanWidget::on_loadTuneDataButton_clicked()
{
	emit loadTuneDataSignal();
}

void ConeScanWidget::on_coneJointLoadTuneDataButton_clicked()
{
	emit loadConeJointTuneDataSignal();
}

void ConeScanWidget::on_saveConeScanConfigButton_clicked()
{
	emit saveConeScanConfigSignal();
}

void ConeScanWidget::on_loadCurrentScanConfigButton_clicked()
{
	emit loadConeScanConfigSignal();
}
