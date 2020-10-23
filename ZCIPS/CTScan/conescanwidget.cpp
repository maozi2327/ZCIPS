#include "stdafx.h"
#include "conescanwidget.h"

ConeScanWidget::ConeScanWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
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
	emit coneScanBeginSignal();
}
 
void ConeScanWidget::on_frameShotButton_clicked()
{
	emit frameShotSignal();
}