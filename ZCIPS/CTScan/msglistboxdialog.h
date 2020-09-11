#pragma once

#include <QDialog>
#include "ui_msglistbox.h"
#include "../Public/util/iulog.h"

class MsgListBoxDialog : public QDialog
{
	Q_OBJECT

public:
	MsgListBoxDialog(const QString& _logFileName, QWidget *parent = Q_NULLPTR);
	~MsgListBoxDialog();
	void logError(QString& _msg);
	void logInfo(QString& _msg);
	void logBug(QString& _msg);
private:
	Ui::MsgListBox ui;

protected:
	void closeEvent(QCloseEvent *event) override;
};
