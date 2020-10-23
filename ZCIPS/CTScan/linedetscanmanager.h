#pragma once

#include <QObject>

#include <memory>
#include <vector>
#include <map>
#include "../Public/headers/setupdata.h"
#include "scantemplate.h"

struct CT3Data;
class LineDetScanInterface;
class ControllerInterface;
class LineDetNetWork;
class CT3TemplateDialog;
class LineDetAirDisposeDialog;
class LineDetScanManager;
class LineDetScanWidget;

class LineDetScanManager : public QObject
{
	Q_OBJECT
private:
	SetupData* d_setupData;
	ConfigData* d_configData;
	LineDetNetWork* d_lineDetNetWork;
	ControllerInterface* d_controller;
	QTimer* d_timer;
	unsigned short d_rayNum;
	unsigned short d_detNum;
	std::map<ScanMode, LineDetScanInterface*> d_scanMap;
	std::unique_ptr<LineDetScanInterface> d_scan;
	CT3Data d_ct3Data;
	LineDetAirDisposeDialog* d_airDisposeDialog;
	CT3TemplateDialog* d_ct3TemplateDialog;
	Ct3TemplateData d_ct3TemplateDataItem;
	LineDetScanWidget* d_lineDetScanWidget;
public:
	LineDetScanManager(int _rayId, int _lineDetId, 
		const std::vector<ScanMode>& _scanMode, SetupData* _setupData, 
		LineDetNetWork* _lineDetNetWork, ControllerInterface* _controller, 
		QWidget *widgetParent = nullptr, QObject *objectParent = nullptr);
	~LineDetScanManager();
	QWidget* getWidget();

private slots:
	void ct3ScanSlot();
	void updateControlsSlot();
	void stopButtonSlot();
	void LoadCt3TemplateButtonSlot();
	void useCt3ItemSlot();
signals:
	void readyToScanSignal(bool _sts);
};
