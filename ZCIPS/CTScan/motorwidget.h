#pragma once

#include <QWidget>
#include "ui_motorwidget.h"
#include <map>
#include <tuple>
#include "..\Public\headers\machineType.h"

class ControllerInterface;
class MotorWidget : public QWidget
{
	Q_OBJECT

public:
	MotorWidget(ControllerInterface* in_controller, QWidget *parent = Q_NULLPTR);
	~MotorWidget();

private:
	Ui::MotorWidget ui;
	ControllerInterface* d_controller;
	void updateStatus();
	void updateAxisStatus();
	QTimer* d_timer;
	QButtonGroup* d_axisRadioButtonGroup;
	Axis d_axisSelected;
	std::map<QRadioButton*, std::tuple<Axis, QLabel*, QLabel*>> d_radioMap;
private slots:
	void on_negativePosButton_clicked();
	void on_positivePosButton_clicked();
	void on_stopButton_clicked();
	void on_absPosButton_clicked();
	void onRadionButtonClicked(QAbstractButton* d_button);
};
