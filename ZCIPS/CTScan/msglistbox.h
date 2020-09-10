#pragma once

#include <QWidget>
#include "ui_msglistbox.h"
#include "../Public/util/iulog.h"

class MsgListBox : public QWidget
{
	Q_OBJECT

public:
	MsgListBox(QString& _logFileName, QWidget *parent = Q_NULLPTR);
	~MsgListBox();
	void logError(QString& _msg);
	void logInfo(QString& _msg);
	void logBug(QString& _msg);
private:
	Ui::MsgListBox ui;

protected:
	void closeEvent(QCloseEvent * event);
};
