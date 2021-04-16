#include "stdafx.h"
#include "loadtunedatadialog.h"
#include "../Public/util/functions.h"

LoadTuneDataDialog::LoadTuneDataDialog(const QString& _airDataDirectory,
	QString& _airName, QWidget *parent)
	: QDialog(parent), d_airDataDirectory(_airDataDirectory)
	, d_airName(_airName)
{
	ui.setupUi(this);
	loadAirNames();
}

LoadTuneDataDialog::~LoadTuneDataDialog()
{
}

void LoadTuneDataDialog::loadAirNames()
{
	browseAllFiles(d_airDataDirectory, d_airNames);

	for (auto& name : d_airNames)
	{
		auto pureName = getPureFileNameFromFullName(name);
		int index = pureName.indexOf(QString::fromLocal8Bit("_55AA5A"));
		pureName = pureName.left(index);
		ui.airNamesListWidget->addItem(pureName);
	}
}

void LoadTuneDataDialog::on_okButton_clicked()
{
	d_airName = d_airDataDirectory + d_airNames[ui.airNamesListWidget->currentRow()];
	accept();
}
