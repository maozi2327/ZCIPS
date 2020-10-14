#pragma once

#include <QtWidgets/QDialog>
#include "ui_siyuan.h"

class SiYuan : public QDialog
{
	Q_OBJECT

public:
	SiYuan(QWidget *parent = Q_NULLPTR);

private:
	Ui::SiYuanClass ui;
};
