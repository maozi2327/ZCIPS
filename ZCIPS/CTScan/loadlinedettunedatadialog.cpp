#include "stdafx.h"
#include "loadlinedettunedatadialog.h"
#include "../Public/util/functions.h"

LoadLineDetTuneDataDialog::LoadLineDetTuneDataDialog(const QString& _bkgDataDirectroy, const QString& _airDataDirectory, 
	QString& _airName, QString& _bkgName, QWidget *parent)
	: QDialog(parent), d_airDataDirectory(_airDataDirectory), d_bkgDataDirectory(_bkgDataDirectroy)
	, d_airName(_airName), d_bkgName(_bkgName)

{
	ui.setupUi(this);
	loadAirNames();
}

LoadLineDetTuneDataDialog::~LoadLineDetTuneDataDialog()
{
}

void LoadLineDetTuneDataDialog::loadAirNames()
{
	browseAllFiles(d_airDataDirectory, d_airNames);

	for (auto& name : d_airNames)
	{
		auto pureName = getPureFileNameFromFullName(name);
		ui.airNamesListWidget->addItem(pureName);
	}
}

void LoadLineDetTuneDataDialog::on_okButton_clicked()
{
	d_airName = d_airDataDirectory + d_airNames[ui.airNamesListWidget->currentRow()];
	accept();
}
