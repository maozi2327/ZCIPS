#include "stdafx.h"
#include "filepathsettingdialog.h"
#include "../Public/util/messagebox.h"

FilePathSettingDialog::FilePathSettingDialog(QString& _orgPath, QString& _disposedFilePath, QWidget *parent)
	: QDialog(parent), d_orgPath(_orgPath), d_disposedFilePath(_disposedFilePath)
{
	ui.setupUi(this);
	ui.orgPathEdit->setText(d_orgPath);
	ui.disposedFilePathEdit->setText(d_disposedFilePath);
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

void FilePathSettingDialog::on_disposedFilePathButton_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择数据文件路径"), "D:/",
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (path.size() != 0)
		ui.disposedFilePathEdit->setText(path);
}

void FilePathSettingDialog::on_okButton_clicked()
{
	QDir dir;

	if (!dir.exists(ui.orgPathEdit->text()) || !dir.exists(ui.disposedFilePathEdit->text()))
	{
		messageBox(QString::fromLocal8Bit("路径错误！"), QString::fromLocal8Bit("重新设置路径！"));
		return;
	}

	d_orgPath = ui.orgPathEdit->text();
	d_disposedFilePath = ui.disposedFilePathEdit->text();
	accept();
}

void FilePathSettingDialog::on_cancelButton_clicked()
{
	reject();
}