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
	ConeScanWidget(Panel* _panel, QWidget* _panelWidget, QWidget *parent = Q_NULLPTR);

	~ConeScanWidget();
	void setPanelDetWidget();
private:
	Ui::ConeScanWidget ui;
	void setConeScanProgress(float _progress, const QString& _msg);
	void initiliseConeScanControls(ConeScanData& _data);
	void initiliseConeJointRotScanControls(ConeJointRotScanData& _data);
	unsigned short getGainFactor(const QString& _text);
	Panel* d_panel;
	QVBoxLayout* d_middleLayout;
	QWidget* d_panelWidget;
	int d_initialHeight;
private slots:
	void on_coneScanBeginSampleButton_clicked();
	void on_frameShotButton_clicked();
	void on_coneScanStopButton_clicked();
signals:
	void coneScanBeginSignal();
	void frameShotSignal();
	void stopControllerSignal();
};
