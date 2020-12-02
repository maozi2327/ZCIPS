#pragma once
#include <QWidget>
#include "ui_varianwidget.h"

class VarianWidget : public QWidget
{
	Q_OBJECT

public:
	VarianWidget(QWidget *parent = Q_NULLPTR);
	~VarianWidget();

private:
	Ui::VarianWidget ui;
};
