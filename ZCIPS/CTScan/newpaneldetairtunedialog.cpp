#include "stdafx.h"
#include "newpaneldetairtunedialog.h"
#include "panelairtune.h"
#include "../Public/util/functions.h"
#include "../Public/util/messagebox.h"
#include <vector>

NewPanelDetAirTuneDialog::NewPanelDetAirTuneDialog(const QString& _bkgPath, const QString& _orgPath, const QString& _filePath, 
	PanelAirTune* panelAirTune, int _cycleTime,	unsigned short _gainFactor,QWidget *parent)
	: QDialog(parent), d_bkgPath(_bkgPath), d_cycleTime(_cycleTime), d_gainFactor(_gainFactor), d_panelAirTune(panelAirTune)
	, d_orgPath(_orgPath), d_tunedFilePath(_filePath)
{
	ui.setupUi(this);
	setLineEditValidaterNoSpecialChar(ui.nameEdit);
	loadBKgNames();
}

NewPanelDetAirTuneDialog::~NewPanelDetAirTuneDialog()
{
}


void NewPanelDetAirTuneDialog::on_startButton_clicked()
{
	d_frames = ui.framesSpinBox->value();
	QString fileName = ui.nameEdit->text();

	if (fileName.size() == 0)
	{
		messageBox(QString::fromLocal8Bit("空气数据名称为空！"), QString::fromLocal8Bit("输入空气数据名称！"));
		return;
	}

	QString bkgName = ui.bkgNameListWidget->currentItem()->text();
	QString orgName = d_orgPath + fileName + QString::fromLocal8Bit("_55AA5A_") + bkgName + QString::fromLocal8Bit(".tif");
	d_panelAirTune->setFileName(orgName, d_tunedFilePath);
	d_panelAirTune->setBkgFileName(d_bkgPath + bkgName + QString::fromLocal8Bit(".tif"));
	connect(d_panelAirTune, &PanelAirTune::frameAcquiredSignal, this, &NewPanelDetAirTuneDialog::on_updateSampleProgress_slot);
	d_panelAirTune->beginAcquire(d_frames, d_cycleTime, d_gainFactor);
	ui.startButton->setEnabled(false);
}

void NewPanelDetAirTuneDialog::loadBKgNames()
{
	std::vector<QString> bkgFileNames;
	browseAllFiles(d_bkgPath, bkgFileNames);

	for (auto& name : bkgFileNames)
		ui.bkgNameListWidget->addItem(getPureFileNameFromFullName(name));
}

void NewPanelDetAirTuneDialog::on_stopButton_clicked()
{
	d_panelAirTune->stopAcquire();
	ui.startButton->setEnabled(true);
}

void NewPanelDetAirTuneDialog::on_updateSampleProgress_slot(int _framesAcquiredThisRound, int _framesThisRound, int _framesAcquiredAll, int _framesALL, QString _message)
{
	ui.progressBar->setValue(_framesAcquiredThisRound * 100 / _framesThisRound);

	if (_framesAcquiredThisRound == _framesThisRound)
		ui.startButton->setEnabled(true);
}
