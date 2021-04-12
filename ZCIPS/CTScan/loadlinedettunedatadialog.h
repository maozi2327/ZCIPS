#pragma once

#include <QDialog>
#include "ui_loadlinedettunedatadialog.h"

class LoadLineDetTuneDataDialog : public QDialog
{
	Q_OBJECT

public:
	LoadLineDetTuneDataDialog(const QString& _bkgDataDirectroy, const QString& _airDataDirectory, QString& airName, QString& bkgName, QWidget *parent = Q_NULLPTR);
	~LoadLineDetTuneDataDialog();

private:
	Ui::LoadLineDetTuneDataDialog ui;
	QString d_airDataDirectory;
	QString d_bkgDataDirectory;
	std::vector<QString> d_airNames;
	QString& d_airName;
	QString& d_bkgName;
	void loadAirNames();

private slots:
	void on_okButton_clicked();
};
