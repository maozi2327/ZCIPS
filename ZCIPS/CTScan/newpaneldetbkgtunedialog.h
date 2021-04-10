#pragma once

#include <QDialog>
#include "ui_newpaneldetbkgtunedialog.h"

class PanelBkgTune;

class NewPanelDetBkgTuneDialog : public QDialog
{
	Q_OBJECT

public:
	NewPanelDetBkgTuneDialog(const QString& _orgPath, const QString& _filePath, PanelBkgTune* panelBkgTune, int _cycleTime, 
		unsigned short _gainFactor, QWidget *parent = Q_NULLPTR);
	~NewPanelDetBkgTuneDialog();

private:
	Ui::NewPanelDetBkgTuneDialog ui;
	PanelBkgTune* d_panelBkgTune;
	QString d_orgPath;
	QString d_tunedFilePath;
	int d_cycleTime;
	unsigned short d_gainFactor;
	int d_frames;

signals:
	void updateProgressSignal(int progress);
private slots:
	void on_startButton_clicked();
	void on_stopButton_clicked();
	void on_updateSampleProgress_slot(int _framesAcquiredThisRound, int _framesThisRound, int _framesAcquiredAll, int _framesALL);
};
