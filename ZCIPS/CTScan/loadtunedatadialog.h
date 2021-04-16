#pragma once

#include <QDialog>
#include "ui_loadtunedatadialog.h"
#include <vector>

class LoadTuneDataDialog : public QDialog
{
	Q_OBJECT

public:
	LoadTuneDataDialog(const QString& _airDataDirectory, QString& airName, QWidget *parent = Q_NULLPTR);
	~LoadTuneDataDialog();

private:
	Ui::LoadTuneDataDialog ui;
	QString d_airDataDirectory;
	std::vector<QString> d_airNames;
	QString& d_airName;
	void loadAirNames();

private slots:
	void on_okButton_clicked();
};
