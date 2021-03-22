#pragma once

#include <QWidget>
#include "ui_newlinedetbkgtunedialog.h"

class LineDetScanInterface;
class NewLineDetBkgTuneDialog : public QWidget
{
	Q_OBJECT

public:
	NewLineDetBkgTuneDialog(LineDetScanInterface* _airScan, QWidget *parent = Q_NULLPTR);
	~NewLineDetBkgTuneDialog();

private:
	Ui::NewLineDetBkgTuneDialog ui;
	LineDetScanInterface* d_bkgScan;

private slots:
	void on_startButton_clicked();
	void on_stopButton_clicked();
};
