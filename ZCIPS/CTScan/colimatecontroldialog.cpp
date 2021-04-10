#include "stdafx.h"
#include "colimatecontroldialog.h"
#include "leisaibac332e.h"

int ColimateControlDialog::d_dialogCount = 0;

ColimateControlDialog::ColimateControlDialog(LeiSaiBac332E* _leiSai, QWidget *parent)
	: QDialog(parent), d_leiSai(_leiSai)
{
	ui.setupUi(this);
	redPalette.setColor(QPalette::Background, QColor(254, 73, 73));
	greenPalette.setColor(QPalette::Background, QColor(73, 253, 73));
	d_timer = new QTimer(this);
	d_timer->start(100);
	on_rayHRadioButton_toggled();
	ui.rayHRadioButton->setChecked(true);
	connect(d_timer, &QTimer::timeout, this, &ColimateControlDialog::updateCollimaterStatusSlot);
	++d_dialogCount;
}

ColimateControlDialog::~ColimateControlDialog()
{
	--d_dialogCount;
}

int ColimateControlDialog::getDialogCount()
{
	return d_dialogCount;
}

void ColimateControlDialog::updateCollimaterStatusSlot()
{
	LeiSaiStatus motorSts = d_leiSai->readStatus();

	ui.rayHPositiveLimitLabel->setPalette(motorSts.axesStatus[0].elPositive == TRUE ? redPalette : greenPalette);
	ui.rayHNegativeLimitLabel->setPalette(motorSts.axesStatus[0].elNegative == TRUE ? redPalette : greenPalette);
	ui.rayHZeroLabel->setPalette(motorSts.axesStatus[0].homed == TRUE ? greenPalette : redPalette);
	ui.rayHPositionLabel->setText(QString::number(motorSts.axesStatus[0].position * 2, 'f', 1));


	ui.rayVPositiveLimitLabel->setPalette(motorSts.axesStatus[1].elPositive == TRUE ? redPalette : greenPalette);
	ui.rayVNegativeLimitLabel->setPalette(motorSts.axesStatus[1].elNegative == TRUE ? redPalette : greenPalette);
	ui.rayVZeroLabel->setPalette(motorSts.axesStatus[1].homed == TRUE ? greenPalette : redPalette);
	ui.rayVPositionLabel->setText(QString::number(motorSts.axesStatus[1].position * 2, 'f', 1));

	ui.detHPositiveLimitLabel->setPalette(motorSts.axesStatus[2].elPositive == TRUE ? redPalette : greenPalette);
	ui.detHNegativeLimitLabel->setPalette(motorSts.axesStatus[2].elNegative == TRUE ? redPalette : greenPalette);
	ui.detHZeroLabel->setPalette(motorSts.axesStatus[2].homed == TRUE ? greenPalette : redPalette);
	ui.detHPositionLabel->setText(QString::number(motorSts.axesStatus[2].position * 2, 'f', 1));

	ui.detVPositiveLimitLabel->setPalette(motorSts.axesStatus[3].elPositive == TRUE ? redPalette : greenPalette);
	ui.detVNegativeLimitLabel->setPalette(motorSts.axesStatus[3].elNegative == TRUE ? redPalette : greenPalette);
	ui.detVZeroLabel->setPalette(motorSts.axesStatus[3].homed == TRUE ? greenPalette : redPalette);
	ui.detVPositionLabel->setText(QString::number(motorSts.axesStatus[3].position * 2, 'f', 1));

	ui.layerThicknessPositiveLimitLabel->setPalette(motorSts.axesStatus[4].elPositive == TRUE ? redPalette : greenPalette);
	ui.layerThicknessNegativeLimitLabel ->setPalette(motorSts.axesStatus[4].elNegative == TRUE ? redPalette : greenPalette);
	ui.layerThicknessZeroLabel->setPalette(motorSts.axesStatus[4].homed == TRUE ? greenPalette : redPalette);
	ui.layerThicknessPositionLabel->setText(QString::number(-motorSts.axesStatus[4].position * 2, 'f', 1));
}

void ColimateControlDialog::on_rayHRadioButton_toggled()
{
	d_axisChoice = 0;
}

void ColimateControlDialog::on_rayVRadioButton_toggled()
{
	d_axisChoice = 1;
}

void ColimateControlDialog::on_detHRadioButton_toggled()
{
	d_axisChoice = 2;
}

void ColimateControlDialog::on_detVRadioButton_toggled()
{
	d_axisChoice = 3;
}

void ColimateControlDialog::on_layerThicknessRadioButton_toggled()
{
	d_axisChoice = 4;
}

void ColimateControlDialog::on_absPosButton_clicked()
{
	float pos = ui.positionEdit->text().toFloat();

	if (d_axisChoice == 4)
		d_leiSai->aixAbsMove(d_axisChoice, -pos);
	else
		d_leiSai->aixAbsMove(d_axisChoice, pos / 2);
}

void ColimateControlDialog::on_homeButton_clicked()
{
	d_leiSai->homeAxis(d_axisChoice);
}

void ColimateControlDialog::on_stopButton_clicked()
{
	d_leiSai->stopAllAxes();
}
