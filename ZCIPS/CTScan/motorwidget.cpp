#include "stdafx.h"
#include "motorwidget.h"
#include "controllerinterface.h"

MotorWidget::MotorWidget(ControllerInterface* _controller, QWidget *parent)
	: QWidget(parent)
	, d_controller(_controller)
{
	ui.setupUi(this);	
	d_timer = new QTimer(this);
	d_timer->start(100);
	connect(d_timer, &QTimer::timeout, this, &MotorWidget::updateStatus);
	d_axisRadioButtonGroup = new QButtonGroup(this);
	d_axisRadioButtonGroup->setExclusive(true);
	d_axisRadioButtonGroup->addButton(ui.rayLayerRadioButton);
	d_axisRadioButtonGroup->addButton(ui.objRotationRadioButton);
	d_axisRadioButtonGroup->addButton(ui.objTranslationRadioButton);
	d_axisRadioButtonGroup->addButton(ui.objRadialRadioButton);
	d_axisRadioButtonGroup->addButton(ui.detLayerRadioButton);
	d_axisRadioButtonGroup->addButton(ui.detRadialRadioButton);
	d_axisRadioButtonGroup->addButton(ui.detTranslationRadioButton);
	ui.objRotationRadioButton->setChecked(true);
	d_axisSelected = Axis::objRotation;
	d_radioMap.insert({ ui.rayLayerRadioButton,	
		std::make_tuple(Axis::rayLayer, ui.rayLayerPosLabel, ui.rayLayerLabel) });
	d_radioMap.insert({ ui.objTranslationRadioButton,
		std::make_tuple(Axis::objTranslation, ui.objTranslationPosLabel, ui.objTranslationLabel) });
	d_radioMap.insert({ ui.objRotationRadioButton,
		std::make_tuple(Axis::objRotation, ui.objRotationPosLabel, ui.objRotationLabel) });
	d_radioMap.insert({ ui.objRadialRadioButton,
		std::make_tuple(Axis::objRadial, ui.objRadialPosLabel, ui.objRadialLabel) });
	d_radioMap.insert({ ui.detLayerRadioButton,
		std::make_tuple(Axis::detLayer, ui.detLayerPosLabel, ui.detLayerLabel) });
	d_radioMap.insert({ ui.detTranslationRadioButton,	
		std::make_tuple(Axis::detTranslation, ui.detTranslationPosLabel, ui.detTranslationLabel) });
	connect(d_axisRadioButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
		this, SLOT(onRadionButtonClicked(QAbstractButton*)));
}

MotorWidget::~MotorWidget()
{
}

void MotorWidget::updateAxisStatus()
{
	std::map<AxisPosEnum, float> axisPos = d_controller->readAxisPostion();
	ui.rayLayerPosLabel->setText(QString("%1").arg(axisPos[AxisPosEnum::rayLayer], 0, 'f', 2));
	ui.objRadialPosLabel->setText(QString("%1").arg(axisPos[AxisPosEnum::objRadial], 0, 'f', 2));
	ui.objRotationPosLabel->setText(QString("%1").arg(axisPos[AxisPosEnum::objRotation], 0, 'f', 2));
	ui.objTranslationPosLabel->setText(QString("%1").arg(axisPos[AxisPosEnum::objTranslation], 0, 'f', 2));
	ui.detLayerPosLabel->setText(QString("%1").arg(axisPos[AxisPosEnum::detLayer], 0, 'f', 2));
	ui.detRadialPosLabel->setText(QString("%1").arg(axisPos[AxisPosEnum::detRadial], 0, 'f', 2));
	ui.detTranslationPosLabel->setText(QString("%1").arg(axisPos[AxisPosEnum::detTranslation], 0, 'f', 2));
}
void MotorWidget::on_negativePosButton_clicked()
{
	d_controller->axisRelMove(d_axisSelected, -ui.posLineEdit->text().toFloat());
}
void MotorWidget::on_positivePosButton_clicked()
{
	d_controller->axisRelMove(d_axisSelected, ui.posLineEdit->text().toFloat());
}
void MotorWidget::on_stopButton_clicked()
{
	d_controller->stopAll();
}
void MotorWidget::on_absPosButton_clicked()
{
	d_controller->axisAbsMove(d_axisSelected, ui.posLineEdit->text().toFloat());
}
void MotorWidget::onRadionButtonClicked(QAbstractButton * d_button)
{
	d_axisSelected = std::get<Axis>(d_radioMap[static_cast<QRadioButton*>(d_button)]);
}
void MotorWidget::updateStatus()
{
	updateAxisStatus();
}
