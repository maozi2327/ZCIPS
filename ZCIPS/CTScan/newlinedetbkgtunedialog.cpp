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
	setAttribute(Qt::WA_DeleteOnClose, true);
}

NewLineDetBkgTuneDialog::~NewLineDetBkgTuneDialog()
{
}


void NewLineDetBkgTuneDialog::on_startButton_clicked()
{
	d_bkgScan->setFileName(d_orgPath + QString::fromLocal8Bit("bkg.org"), d_filePath);
	connect(d_bkgScan, &LineDetBkgTune::samplePercentCountSignal, this, &NewLineDetBkgTuneDialog::on_updateProgressSlot);
	d_bkgScan->beginScan();
}

void NewLineDetBkgTuneDialog::on_stopButton_clicked()
{
	d_bkgScan->stopScan();
}

void NewLineDetBkgTuneDialog::on_updateProgressSlot(int _progress)
{
	ui.progressBar->setValue(float(_progress));
}