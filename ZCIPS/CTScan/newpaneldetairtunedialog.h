#pragma once

#include <QDialog>
#include "ui_newpaneldetairtunedialog.h"

class PanelAirTune;

class NewPanelDetAirTuneDialog : public QDialog
{
	Q_OBJECT

public:
	NewPanelDetAirTuneDialog(const QString& _bkgPath, const QString& _orgPath, const QString& _filePath, PanelAirTune* panelAirTune, int _cycleTime,
		unsigned short _gainFactor, QWidget *parent = Q_NULLPTR);
	~NewPanelDetAirTuneDialog();

private:
	Ui::NewPanelDetAirTuneDialog ui;
	PanelAirTune* d_panelAirTune;
	QString d_orgPath;
	QString d_tunedFilePath;
	QString d_bkgPath;
	int d_cycleTime;
	unsigned short d_gainFactor;
	int d_frames;
	void loadBKgNames();
signals:
	void updateProgressSignal(int progress);
private slots:
	void on_startButton_clicked();
	void on_stopButton_clicked();
	void on_updateSampleProgress_slot(int _framesAcquiredThisRound, int _framesThisRound, int _framesAcquiredAll, int _framesALL, QString _message);

};
