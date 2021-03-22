#include "stdafx.h"
#include "addmodifyairdisposedialog.h"
#include "LineDetAirTune.h"
#include "../Public/util/messagebox.h"

AddModifyAirDisposeDialog* AddModifyAirDisposeDialog::d_ref = nullptr;

AddModifyAirDisposeDialog::AddModifyAirDisposeDialog(const QString& _orgPath, const QString& _filePath, 
	LineDetScanInterface* _airScan, QWidget *parent)
	: d_airScan(_airScan), QDialog(parent)
	, d_orgPath(_orgPath), d_filePath(_filePath)
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
	QString fileName = ui.nameLineEdit->text() + QString::fromLocal8Bit("_") + ui.voltageLineEdit->text() + 
		QString::fromLocal8Bit("_") + ui.currentLineEdit->text() + 
		QString::fromLocal8Bit("_") + ui.ct3LayerThickness_2->text() + 
		QString::fromLocal8Bit("_") + ui.comentEdit->text();
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

void AddModifyAirDisposeDialog::on_nameLineEdit_textChanged()
{

}

void AddModifyAirDisposeDialog::on_voltageLineEdit_textChanged()
{

}

void AddModifyAirDisposeDialog::on_ct3LayerThickness_2_textChanged()
{

}

void AddModifyAirDisposeDialog::on_comentEdit_textChanged()
{

}
