#pragma once

#include <QWidget>
#include <memory>
#include "ui_conescanwidget.h"
#include "../Public/headers/setupdata.h"

class PanelImageProcess;
class ConeScanInterface;
class ControllerInterface;
class MotorControlWidget;
class Panel;

class ConeScanWidget : public QWidget
{
	Q_OBJECT

public:
	ConeScanWidget(MotorControlWidget* _motorControl, int _rayId, int _panelDetId, const
		std::vector<ScanMode>& _scanMode, SetupData* _setupData,
		Panel* _panel, ControllerInterface* _controller, QWidget *parent = Q_NULLPTR);

	~ConeScanWidget();

private:
	Ui::ConeScanWidget ui;
	std::unique_ptr<ConeScanInterface> d_scan;
	std::unique_ptr<PanelImageProcess> d_panelImageProcess;
	unsigned short d_rayNum;
	unsigned short d_detNum;
	MotorControlWidget* d_motorControlDialog;
	SetupData* d_setupData;
	ControllerInterface* d_controller;
	Panel* d_panel;
	void setConeScanProgress(int _progress, const QString& _msg);
	void initiliseConeScanControls(ConeScanData& _data);
	void initiliseConeJointRotScanControls(ConeJointRotScanData& _data);
private slots:
	void on_coneScanBeginSampleButton_clicked();
	void on_scanProgressUpdated();
};
