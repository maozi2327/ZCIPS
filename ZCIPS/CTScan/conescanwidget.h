#pragma once

#include <QWidget>
#include <memory>
#include "ui_conescanwidget.h"
#include "../Public/headers/setupdata.h"

class Panel;
class ConeScanWidget : public QWidget
{
	Q_OBJECT
	
	friend class PanelDetScanManager;

public:
	ConeScanWidget(Panel* _panel, QWidget *parent = Q_NULLPTR);

	~ConeScanWidget();

private:
	Ui::ConeScanWidget ui;
	void setConeScanProgress(int _progress, const QString& _msg);
	void initiliseConeScanControls(ConeScanData& _data);
	void initiliseConeJointRotScanControls(ConeJointRotScanData& _data);
	Panel* d_panel;
private slots:
	void on_coneScanBeginSampleButton_clicked();
	void on_frameShotButton_clicked();
	void on_gainGroupBox_currentIndexChanged(int _index);
	void on_cycleTimeEdit_returnPressed();
signals:
	void coneScanBeginSignal();
	void frameShotSignal();
};
