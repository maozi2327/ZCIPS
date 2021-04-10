#pragma once

#include <QDialog>
#include "ui_linedetfilenamedialog.h"

class LineDetFileNameDialog : public QDialog
{
	Q_OBJECT

public:
	LineDetFileNameDialog(QString& _objectName, QString& _objectNumber, QString& _fileNumber, QString& _comment, QWidget *parent = Q_NULLPTR);
	~LineDetFileNameDialog();

private:
	Ui::LineDetFileNameDialog ui;
	QString& d_objectName;
	QString& d_objectNumber;
	QString& d_fileNumber;
	QString& d_comment;

private slots:
	void on_okButton_clicked();
	void on_cancelButton_clicked();
};
