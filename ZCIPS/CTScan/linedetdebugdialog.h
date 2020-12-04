#pragma once

#include <QDialog>
#include "ui_linedetdebugdialog.h"

class LineDetDebugDialog : public QDialog
{
	Q_OBJECT

public:
	LineDetDebugDialog(QWidget *parent = Q_NULLPTR);
	~LineDetDebugDialog();

private:
	Ui::LineDetDebugDialog ui;

protected:
	//virtual void paintEvent(QPaintEvent *event) override;
};
