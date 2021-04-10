#include "stdafx.h"
#include "newlinedetairtunedialog.h"
#include "LineDetAirTune.h"
#include "../Public/util/messagebox.h"
#include "../Public/util/legalinput.h"

NewLineDetAirTuneDialog* NewLineDetAirTuneDialog::d_ref = nullptr;

NewLineDetAirTuneDialog::NewLineDetAirTuneDialog(const QString& _orgPath, const QString& _filePath, 
	LineDetScanInterface* _airScan, QWidget *parent)
	: d_airScan(_airScan), QDialog(parent)
	, d_orgPath(_orgPath), d_filePath(_filePath)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

NewLineDetAirTuneDialog::~NewLineDetAirTuneDialog()
{
	d_ref = nullptr;
}

void NewLineDetAirTuneDialog::on_startButton_clicked()
{
	QString fileName = ui.nameLineEdit->text() + QString::fromLocal8Bit("_") + ui.voltageLineEdit->text() + 
		QString::fromLocal8Bit("_") + ui.currentLineEdit->text() + 
		QString::fromLocal8Bit("_") + ui.ct3LayerThickness_2->text() + 
		QString::fromLocal8Bit("_") + ui.comentEdit->text();
	d_airScan->setFileName(d_orgPath + fileName + QString::fromLocal8Bit(".org"), d_filePath);
	connect(d_airScan, &LineDetAirTune::samplePercentCountSignal, this, &NewLineDetAirTuneDialog::on_updateProgressSlot);
	d_airScan->beginScan();
}

void NewLineDetAirTuneDialog::on_stopButton_clicked()
{
	d_airScan->stopScan();

}

void NewLineDetAirTuneDialog::on_updateProgressSlot(int _progress)
{
	ui.progressBar->setValue(float(_progress));
}

void NewLineDetAirTuneDialog::updateScanButtonSlot(bool _sts)
{
	ui.startButton->setEnabled(_sts);
}

void NewLineDetAirTuneDialog::closeEvent(QCloseEvent* _event)
{

}

void NewLineDetAirTuneDialog::on_nameLineEdit_textChanged(QString _str)
{
	if (!legalInputNoneSpecialChar(_str))
	{
		messageBox(QString::fromLocal8Bit("不能包含特殊字符"), QString::fromLocal8Bit("请重新输入"));
		ui.nameLineEdit->clear();
	}	
}

void NewLineDetAirTuneDialog::on_voltageLineEdit_textChanged(QString _str)
{
	if (!legalInputFloatOnly(_str))
	{
		messageBox(QString::fromLocal8Bit("只能输入数字"), QString::fromLocal8Bit("请重新输入"));
		ui.voltageLineEdit->clear();
	}
}

void NewLineDetAirTuneDialog::on_ct3LayerThickness_2_textChanged(QString _str)
{
	if (!legalInputFloatOnly(_str))
	{
		messageBox(QString::fromLocal8Bit("只能输入数字"), QString::fromLocal8Bit("请重新输入"));
		ui.ct3LayerThickness_2->clear();
	}
}

void NewLineDetAirTuneDialog::on_comentEdit_textChanged(QString _str)
{
	if (!legalInputNoneSpecialChar(_str))
	{
		messageBox(QString::fromLocal8Bit("不能包含特殊字符"), QString::fromLocal8Bit("请重新输入"));
		ui.comentEdit->clear();
	}
}
