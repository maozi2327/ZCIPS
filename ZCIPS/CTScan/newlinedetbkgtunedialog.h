#pragma once

#include <QWidget>
#include "ui_newlinedetbkgtunedialog.h"

class LineDetScanInterface;
class NewLineDetBkgTuneDialog : public QDialog
{
	Q_OBJECT

public:
	NewLineDetBkgTuneDialog(const QString& _orgPath, const QString& _filePath, LineDetScanInterface* _bkgScan, QDialog *parent = Q_NULLPTR);
	~NewLineDetBkgTuneDialog();

private:
	Ui::NewLineDetBkgTuneDialog ui;
	LineDetScanInterface* d_bkgScan;
	QString d_orgPath;
	QString d_filePath;

private slots:
	void on_startButton_clicked();
	void on_stopButton_clicked();
	void on_updateProgressSlot(int _progress);
};
