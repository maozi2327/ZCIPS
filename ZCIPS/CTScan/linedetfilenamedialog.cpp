#include "stdafx.h"
#include "linedetfilenamedialog.h"

LineDetFileNameDialog::LineDetFileNameDialog(QString& _objectName, QString& _objectNumber, QString& _fileNumber, 
	QString& _comment, QWidget *parent)
	: QDialog(parent), d_objectName(_objectName), d_objectNumber(_objectNumber), d_fileNumber(_fileNumber), d_comment(_comment)
{
	ui.setupUi(this);
	ui.objectNameEdit->setText(_objectName);
	ui.objectNumberEdit->setText(_objectNumber);
	ui.fileNumberEdit->setText(_fileNumber);
	ui.commentEdit->setText(_comment);
}

LineDetFileNameDialog::~LineDetFileNameDialog()
{

}

void LineDetFileNameDialog::on_cancelButton_clicked()
{
	reject();
}

void LineDetFileNameDialog::on_okButton_clicked()
{
	d_objectName = ui.objectNameEdit->text();
	d_objectNumber = ui.objectNumberEdit->text();
	d_fileNumber = ui.fileNumberEdit->text();
	d_comment = ui.commentEdit->text();

	if(d_objectNumber.size() != 0 && d_objectNumber.size() != 0)
		accept();
}
