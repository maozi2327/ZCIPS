#pragma once
#include <QDialog>
#include "ui_colimatecontroldialog.h"
class LeiSaiBac332E;

class ColimateControlDialog : public QDialog
{
	Q_OBJECT

public:
	ColimateControlDialog(LeiSaiBac332E* _leiSai, QWidget *parent = Q_NULLPTR);
	~ColimateControlDialog();
	static int getDialogCount();

private:
	Ui::colimateControlDialog ui;
	QPalette redPalette;
	QPalette greenPalette;
	QTimer*	d_timer;
	LeiSaiBac332E* d_leiSai;
	int d_axisChoice;
	static int d_dialogCount;

private slots:
	void updateCollimaterStatusSlot();
	void on_rayHRadioButton_toggled();
	void on_rayVRadioButton_toggled();
	void on_detHRadioButton_toggled();
	void on_detVRadioButton_toggled();
	void on_layerThicknessRadioButton_toggled();
	void on_absPosButton_clicked();
	void on_homeButton_clicked();
	void on_stopButton_clicked();
};
