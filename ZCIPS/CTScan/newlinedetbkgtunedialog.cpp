#include "stdafx.h"
#include "newlinedetbkgtunedialog.h"
#include "LineDetBkgTune.h"
#include "../Public/util/messagebox.h"

NewLineDetBkgTuneDialog::NewLineDetBkgTuneDialog(const QString& _orgPath, const QString& _filePath, 
	LineDetScanInterface* _bkgScan, QDialog *parent)
	: d_bkgScan(_bkgScan), QDialog(parent)
	, d_orgPath(_orgPath), d_filePath(_filePath)
{
	ui.setupUi(this);
}

NewLineDetBkgTuneDialog::~NewLineDetBkgTuneDialog()
{
}


void NewLineDetBkgTuneDialog::on_startButton_clicked()
{
	d_bkgScan->setFileName(d_orgPath + QString::fromLocal8Bit("bkg.org"), d_filePath);
	connect(d_bkgScan, &LineDetBkgTune::scanProgressSignal, this, &NewLineDetBkgTuneDialog::on_updateProgressSlot);
	connect(d_bkgScan, &LineDetBkgTune::scanThreadQuitSignal, this, &NewLineDetBkgTuneDialog::scanFinishedSlot);
	d_bkgScan->beginScan();
	ui.startButton->setEnabled(false);
}

void NewLineDetBkgTuneDialog::on_stopButton_clicked()
{
	d_bkgScan->stopScan();
}

void NewLineDetBkgTuneDialog::on_updateProgressSlot(int _graduationAcquiredThisRound, int _graduationThisRound, int _graduationAcquiredAll, int _graduationALL, QString message)
{
	ui.progressBar->setValue(100 * _graduationAcquiredThisRound / _graduationThisRound);
	ui.startButton->setEnabled(false);
}

void NewLineDetBkgTuneDialog::scanFinishedSlot(int _flag)
{
	ui.startButton->setEnabled(true);
}

