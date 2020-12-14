#pragma once

#include <QDialog>
#include "ui_linedetdebugdialog.h"

class LineDetNetWork;
class DrawWidget;

class LineDetDebugDialog : public QDialog
{
	Q_OBJECT

public:
	LineDetDebugDialog(LineDetNetWork* _lineDetNetWork, QWidget *parent = Q_NULLPTR);
	~LineDetDebugDialog();

private:
	Ui::LineDetDebugDialog ui;
	LineDetNetWork* d_lineDetWork;
	int d_channelNum;
	DrawWidget* d_graph;
	QTimer* d_timer;
	
	QTimer* d_TESTtimer;

	unsigned long* d_data;
protected:
	//virtual void paintEvent(QPaintEvent *event) override;

private slots:
	void on_downloadSampleTimeButton_clicked();
	void on_downloadDelayTimeButton_clicked();
	void on_downloadAmplifyFactorButton_clicked();
	void on_uploadSampleTimeButton_clicked();
	void on_uploadAmplifyFactorButton_clicked();
	void on_uploadDelayTimeButton_clicked();
	void on_setDepthButton_clicked();
	void on_setMaskButton_clicked();
	void on_internalAcquireButton_clicked();
	void on_stopInternalAcquireButton_clicked();
	void updateDataSlot();
	void updateDataSlot1();
};
