#include "stdafx.h"
#include "loadtunedatadialog.h"
#include "../Public/util/functions.h"

LoadTuneDataDialog::LoadTuneDataDialog(const QString& _bkgDataDirectroy, const QString& _airDataDirectory,
	QString& _airName, QString& _bkgName, QWidget *parent)
	: QDialog(parent), d_airDataDirectory(_airDataDirectory), d_bkgDataDirectory(_bkgDataDirectroy)
	, d_airName(_airName), d_bkgName(_bkgName)
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
	QString bkgName = d_bkgDataDirectory + 
		d_airName.right(d_airName.size() - (d_airName.indexOf(QString::fromLocal8Bit("_55AA5A_")) + QString::fromLocal8Bit("_55AA5A_").size()));
	d_bkgName = bkgName;
	accept();
}
