#include "stdafx.h"
#include "newpaneldetbkgtunedialog.h"
#include "panelbkgtune.h"
#include "../Public/util/messagebox.h"

NewPanelDetBkgTuneDialog::NewPanelDetBkgTuneDialog(const QString& _orgPath, const QString& _filePath, PanelBkgTune* panelBkgTune, 
	int _cycleTime, unsigned short _gainFactor, QWidget *parent)
	: QDialog(parent), d_orgPath(_orgPath), d_tunedFilePath(_filePath), d_panelBkgTune(panelBkgTune)
	, d_cycleTime(_cycleTime), d_gainFactor(_gainFactor)
{
	ui.setupUi(this);
	QRegExp rx = QRegExp("[^\\\\/:*?\"<>|_]*");
	QRegExpValidator* validator = new QRegExpValidator(rx);
	ui.nameEdit->setValidator(validator);
}

NewPanelDetBkgTuneDialog::~NewPanelDetBkgTuneDialog()
{
}

void NewPanelDetBkgTuneDialog::on_startButton_clicked()
{
	d_frames = ui.framesSpinBox->value();
	QString fileName = ui.nameEdit->text();

	if (fileName.size() == 0)
	{
		messageBox(QString::fromLocal8Bit("背景数据名称为空！"), QString::fromLocal8Bit("输入背景数据名称！"));
		return;
	}

	QString orgName = d_orgPath + fileName + QString::fromLocal8Bit(".tif");
	d_panelBkgTune->setFileName(orgName, d_tunedFilePath);
	connect(d_panelBkgTune, &PanelBkgTune::frameAcquiredSignal, this, &NewPanelDetBkgTuneDialog::on_updateSampleProgress_slot);
	d_panelBkgTune->beginAcquire(d_frames, d_cycleTime, d_gainFactor);
	ui.startButton->setEnabled(false);
}

void NewPanelDetBkgTuneDialog::on_stopButton_clicked()
{
	d_panelBkgTune->stopAcquire();
	ui.startButton->setEnabled(true);
}

void NewPanelDetBkgTuneDialog::on_updateSampleProgress_slot(int _framesAcquiredThisRound, int _framesThisRound, int _framesAcquiredAll, int _framesALL)
{
	ui.progressBar->setValue(_framesAcquiredThisRound * 100 / _framesThisRound);

	if(_framesAcquiredThisRound == _framesAcquiredThisRound)
		ui.startButton->setEnabled(true);
}
