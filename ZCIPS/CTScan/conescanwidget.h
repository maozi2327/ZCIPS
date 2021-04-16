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
private:
	Ui::ConeScanWidget ui;
	void setConeScanProgress(int _progress, int _progressAll, const QString& _msg);
	void initiliseConeScanControls(ConeScanData& _data);
	void initiliseConeJointRotScanControls(ConeJointRotScanData& _data);
	unsigned short getGainFactor(const QString& _text);
	Panel* d_panel;
	QVBoxLayout* d_middleLayout;
	QWidget* d_panelWidget;
	int d_initialHeight;

	friend class PanelDetScanManager;
private slots:
	void on_coneScanBeginSampleButton_clicked();
	void on_frameShotButton_clicked();
	void on_coneScanStopButton_clicked();
	void on_previewButton_clicked();
	void on_bkgTuneButton_clicked();
	void on_airTuneButton_clicked();
	void on_loadTuneDataButton_clicked();
	void on_coneJointLoadTuneDataButton_clicked();
	void on_saveConeScanConfigButton_clicked();
signals:
	void saveConeScanConfigSignal();
	void coneScanBeginSignal();
	void coneJointScanBeginSignal();
	void loadConeJointTuneDataSignal();
	void frameShotSignal();
	void coneScanStopSignal();
	void previewSignal();
	void bkgTuneSignal();
	void airTuneSignal();
	void loadTuneDataSignal();

};
