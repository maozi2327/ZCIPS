#pragma once

#include <QDialog>
#include "ui_NewLineDetAirTuneDialog.h"
#include "../Public/headers/setupdata.h"
#include <memory>
#include <vector>
class LineDetScanInterface;

class NewLineDetAirTuneDialog : public QDialog
{
	Q_OBJECT
public:
	NewLineDetAirTuneDialog(const QString& _orgPath, const QString& _filePath, LineDetScanInterface* _airScan, 
		QWidget *parent = Q_NULLPTR);
	~NewLineDetAirTuneDialog();

private:
	Ui::NewLineDetAirTuneDialog ui;
	LineDetScanInterface* d_airScan;
	QString d_orgPath;
	QString d_filePath;

private slots:
	void on_startButton_clicked();
	void on_stopButton_clicked();
	void on_updateProgressSlot(int _graduationAcquiredThisRound, int _graduationThisRound, int _graduationAcquiredAll, int _graduationALL, QString message);
	void scanFinishedSlot();
public slots:
	void updateScanButtonSlot(bool _sts);
};
