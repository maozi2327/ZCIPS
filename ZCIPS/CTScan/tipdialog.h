#pragma once

#include <QWidget>
#include "ui_tipdialog.h"
#include <vector>

class TipDialog : public QDialog
{
	Q_OBJECT

public:
	TipDialog(QDialog *parent = Q_NULLPTR);
	~TipDialog();

	void setTip(const std::vector<QString>& _tip);
	void clearTip();
private:
	Ui::TipDialog ui;
};
