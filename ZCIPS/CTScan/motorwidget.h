#pragma once

#include <QWidget>
#include "ui_motorwidget.h"

class MotorWidget : public QWidget
{
	Q_OBJECT

public:
	MotorWidget(QWidget *parent = Q_NULLPTR);
	~MotorWidget();

private:
	Ui::MotorWidget ui;
};
