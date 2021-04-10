#pragma once

#include <QDialog>
#include "ui_colimatezeroajustdialog.h"
class LeiSaiBac332E;

class ColimateZeroAjustDialog : public QDialog
{
	Q_OBJECT

public:
	ColimateZeroAjustDialog(LeiSaiBac332E* _leiSai, QWidget *parent = Q_NULLPTR);
	~ColimateZeroAjustDialog();
	static int getDialogCount();

private:
	LeiSaiBac332E* d_leisai;
	Ui::ColimateZeroAjustDialog ui;
	static int d_dialogCount;

private slots:
	void on_saveButton_clicked();
};
