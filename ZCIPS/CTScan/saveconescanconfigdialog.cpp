#include "stdafx.h"
#include "saveconescanconfigdialog.h"
#include "../Public/util/functions.h"
#include "loadtunedatadialog.h"

SaveConeScanConfigDialog::SaveConeScanConfigDialog(int _rayIndex, int _panelDetIndex, 
	float _slicePos, float _orient,	float _sod, float _sdd, Panel* _panel, coneScanDataItem *_item, QComboBox* _graduationCombox,
	int _frames, const QString& _airName, const QString& _tunedAirDirectory, QWidget *parent)
	: QDialog(parent), d_panelWidget(_panel->getTempWidget()), d_rayIndex(_rayIndex), d_panelDetIndex(_panelDetIndex), d_item(_item)
	, d_tunedAirDirectory(_tunedAirDirectory), d_panel(_panel), d_airName(_airName)
{
	ui.setupUi(this);
	ui.panelWidget->setLayout(d_panelWidget->layout());

	for (int i = 0; i != _graduationCombox->count(); ++i)
		ui.graduationComboBox->addItem(_graduationCombox->itemText(i));

	ui.graduationComboBox->setCurrentIndex(_graduationCombox->currentIndex());
	ui.framesSpinBox->setValue(_frames);
	setLineEditValidaterFloatChar(std::vector<QLineEdit*>{ ui.slicePosEdit, ui.orientIncEdit, ui.sodEdit, ui.sddEdit });
	auto pureName = getPureFileNameFromFullName(_airName);
	int index = pureName.indexOf(QString::fromLocal8Bit("_55AA5A"));
	pureName = pureName.left(index);
	ui.airNameEdit->setText(pureName);

	ui.slicePosEdit->setText(QString::number(_slicePos, 'f', 2));
	ui.orientIncEdit->setText(QString::number(_orient, 'f', 2));
	ui.sodEdit->setText(QString::number(_sod, 'f', 2));
	ui.sddEdit->setText(QString::number(_sdd, 'f', 2));
}

SaveConeScanConfigDialog::~SaveConeScanConfigDialog()
{
}

void SaveConeScanConfigDialog::on_loadAirDataButton_clicked()
{
	QString airName;
	LoadTuneDataDialog loadTuneDataDialog(d_tunedAirDirectory, airName);

	if (loadTuneDataDialog.exec() == QDialog::Accepted)
	{
		d_airName = airName;
		auto pureName = getPureFileNameFromFullName(airName);
		int index = pureName.indexOf(QString::fromLocal8Bit("_55AA5A"));
		pureName = pureName.left(index);
		ui.airNameEdit->setText(pureName);
	}
}

void SaveConeScanConfigDialog::on_okButton_clicked()
{
	d_item->itemName = ui.configNameEdit->text();
	d_item->rayIndex = d_rayIndex;
	d_item->paneldetIndex = d_panelDetIndex;
	d_item->graduationComboxIndex = ui.graduationComboBox->currentIndex();
	d_item->frames = ui.framesSpinBox->value();
	d_item->slicePos = ui.slicePosEdit->text().toFloat();
	d_item->orient = ui.orientIncEdit->text().toFloat();
	d_item->sod = ui.sodEdit->text().toFloat();
	d_item->sdd = ui.sddEdit->text().toFloat();
	d_item->panelData.cycleTime = d_panel->getTempSampleTimeSet();
	d_item->panelData.gainFactorIndex = d_panel->getTempGainFactorSet();
	d_item->airName = d_airName;
	accept();
}

void SaveConeScanConfigDialog::on_cancelButton_clicked()
{
	rejected();
}
