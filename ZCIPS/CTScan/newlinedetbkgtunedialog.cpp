#include "stdafx.h"
#include "newlinedetbkgtunedialog.h"
#include "LineDetBkgTune.h"
#include "../Public/util/messagebox.h"

NewLineDetBkgTuneDialog::NewLineDetBkgTuneDialog(LineDetScanInterface* _airScan, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

NewLineDetBkgTuneDialog::~NewLineDetBkgTuneDialog()
{
}


void NewLineDetBkgTuneDialog::on_startButton_clicked()
{
	d_bkgScan->beginScan();
}

void NewLineDetBkgTuneDialog::on_stopButton_clicked()
{
	d_bkgScan->stopScan();
}