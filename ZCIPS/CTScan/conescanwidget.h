#pragma once

#include <QWidget>
#include <memory>
#include "ui_conescanwidget.h"
#include "../Public/headers/setupdata.h"

class ConeScanWidget : public QWidget
{
	Q_OBJECT
	
	friend class PanelDetScanManager;

public:
	ConeScanWidget(QWidget *parent = Q_NULLPTR);

	~ConeScanWidget();

private:
	Ui::ConeScanWidget ui;
	void setConeScanProgress(int _progress, const QString& _msg);
	void initiliseConeScanControls(ConeScanData& _data);
	void initiliseConeJointRotScanControls(ConeJointRotScanData& _data);

private slots:
	void on_coneScanBeginSampleButton_clicked();
	void on_frameShotButton_clicked();
signals:
	void coneScanBeginSignal();
	void frameShotSignal();
};
