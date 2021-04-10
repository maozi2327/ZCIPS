#include "stdafx.h"
#include "filepathsettingdialog.h"
#include "../Public/util/messagebox.h"

FilePathSettingDialog::FilePathSettingDialog(QString& _orgPath, QString& _tunedFilePath, QWidget *parent)
	: QDialog(parent), d_orgPath(_orgPath), d_tunedFilePath(_tunedFilePath)
{
	ui.setupUi(this);
	ui.orgPathEdit->setText(d_orgPath);
	ui.tunedFilePathEdit->setText(d_tunedFilePath);
}

FilePathSettingDialog::~FilePathSettingDialog()
{

}


void FilePathSettingDialog::on_orgPathButton_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择原始文件路径"), "D:/",
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(path.size() != 0)
		ui.orgPathEdit->setText(path);
}

void FilePathSettingDialog::on_tunedFilePathButton_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择数据文件路径"), "D:/",
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (path.size() != 0)
		ui.tunedFilePathEdit->setText(path);
}

void FilePathSettingDialog::on_okButton_clicked()
{
	QDir dir;

	if (!dir.exists(ui.orgPathEdit->text()) || !dir.exists(ui.tunedFilePathEdit->text()))
	{
		messageBox(QString::fromLocal8Bit("路径错误！"), QString::fromLocal8Bit("重新设置路径！"));
		return;
	}

	d_orgPath = ui.orgPathEdit->text();
	d_tunedFilePath = ui.tunedFilePathEdit->text();
	accept();
}

void FilePathSettingDialog::on_cancelButton_clicked()
{
	reject();
}