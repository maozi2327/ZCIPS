#include "stdafx.h"
#include "newlinedetairtunedialog.h"
#include "LineDetAirTune.h"
#include "../Public/util/messagebox.h"
#include "../Public/util/functions.h"

NewLineDetAirTuneDialog::NewLineDetAirTuneDialog(const QString& _orgPath, const QString& _filePath, 
	LineDetScanInterface* _airScan, QWidget *parent)
	: d_airScan(_airScan), QDialog(parent)
	, d_orgPath(_orgPath), d_filePath(_filePath)
{
	ui.setupUi(this);
	setLineEditValidaterNoSpecialChar(ui.nameLineEdit);
}

NewLineDetAirTuneDialog::~NewLineDetAirTuneDialog()
{

}

void NewLineDetAirTuneDialog::on_startButton_clicked()
{
	d_airScan->setFileName(d_orgPath + ui.nameLineEdit->text() + QString::fromLocal8Bit(".org"), d_filePath);
	connect(d_airScan, &LineDetAirTune::scanProgressSignal, this, &NewLineDetAirTuneDialog::on_updateProgressSlot);
	connect(d_airScan, &LineDetAirTune::scanThreadQuitSignal, this, &NewLineDetAirTuneDialog::scanFinishedSlot);
	ui.startButton->setEnabled(false);
	d_airScan->beginScan();
}

void NewLineDetAirTuneDialog::on_stopButton_clicked()
{
	d_airScan->stopScan();
}

void NewLineDetAirTuneDialog::on_updateProgressSlot(int _graduationAcquiredThisRound, int _graduationThisRound, int _graduationAcquiredAll, int _graduationALL, QString message)
{
	ui.progressBar->setValue(100 * _graduationAcquiredThisRound/ _graduationThisRound);
}

void NewLineDetAirTuneDialog::scanFinishedSlot()
{
	ui.startButton->setEnabled(true);
}

void NewLineDetAirTuneDialog::updateScanButtonSlot(bool _sts)
{
	ui.startButton->setEnabled(_sts);
}
