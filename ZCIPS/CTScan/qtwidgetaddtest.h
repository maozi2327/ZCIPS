#pragma once

#include <QWidget>
#include "ui_qtwidgetaddtest.h"

class QtWidgetAddTest : public QWidget
{
	Q_OBJECT

public:
	QtWidgetAddTest(QWidget *parent = Q_NULLPTR);
	~QtWidgetAddTest();

private:
	Ui::QtWidgetAddTest ui;
};
