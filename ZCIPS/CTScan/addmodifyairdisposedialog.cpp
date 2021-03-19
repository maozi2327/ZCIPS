#include "stdafx.h"
#include "addmodifyairdisposedialog.h"
#include "LineDetAirTune.h"
#include "linedetairdisposedialog.h"
#include "../Public/util/messagebox.h"

AddModifyAirDisposeDialog* AddModifyAirDisposeDialog::d_ref = nullptr;

AddModifyAirDisposeDialog::AddModifyAirDisposeDialog(LineDetScanInterface* _airScan, QWidget *parent)
	: d_airScan(_airScan), QDialog(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

AddModifyAirDisposeDialog::~AddModifyAirDisposeDialog()
{
	d_ref = nullptr;
}

void AddModifyAirDisposeDialog::on_startButton_clicked()
{
	d_airScan->beginScan();
}

void AddModifyAirDisposeDialog::on_stopButton_clicked()
{
	d_airScan->stopScan();
}

void AddModifyAirDisposeDialog::updateScanButtonSlot(bool _sts)
{
	ui.startButton->setEnabled(_sts);
}

void AddModifyAirDisposeDialog::closeEvent(QCloseEvent* _event)
{

}