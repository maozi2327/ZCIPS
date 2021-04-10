#pragma once

#include <QDialog>
#include "ui_loadtunedatadialog.h"
#include <vector>

class LoadTuneDataDialog : public QDialog
{
	Q_OBJECT

public:
	LoadTuneDataDialog(const QString& _bkgDataDirectroy, const QString& _airDataDirectory, QString& airName, QString& bkgName,
		QWidget *parent = Q_NULLPTR);
	~LoadTuneDataDialog();

private:
	Ui::LoadTuneDataDialog ui;
	QString d_airDataDirectory;
	QString d_bkgDataDirectory;
	std::vector<QString> d_airNames;
	QString& d_airName;
	QString& d_bkgName;
	void loadAirNames();

private slots:
	void on_okButton_clicked();
};
