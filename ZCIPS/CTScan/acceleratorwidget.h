#pragma once

#include <QWidget>
#include "ui_Acceleratorwidget.h"

class AcceleratorWidget : public QWidget
{
	Q_OBJECT

public:
	AcceleratorWidget(QWidget *parent = Q_NULLPTR);
	~AcceleratorWidget();

private:
	Ui::AcceleratorWidget ui;
};
