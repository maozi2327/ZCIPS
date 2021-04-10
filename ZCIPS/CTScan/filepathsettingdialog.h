#pragma once

#include <QDialog>
#include "ui_filepathsettingdialog.h"

class FilePathSettingDialog : public QDialog
{
	Q_OBJECT

public:
	FilePathSettingDialog(QString& _orgPath, QString& _tunedFilePath, QWidget *parent = Q_NULLPTR);
	~FilePathSettingDialog();

private:
	Ui::FilePathSettingDialog ui;
	QString& d_orgPath;
	QString& d_tunedFilePath;
private slots:
	void on_orgPathButton_clicked();
	void on_tunedFilePathButton_clicked();
	void on_okButton_clicked();
	void on_cancelButton_clicked();
};
