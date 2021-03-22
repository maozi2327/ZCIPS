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
class AddModifyAirDisposeDialog;
class LineDetScanManager;
class LineDetScanWidget;
class LinedetFileNameDialog;
class LineDetAirTune;

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
	AddModifyAirDisposeDialog* d_newLineDetAirDisposeDialog;
	CT3TemplateDialog* d_ct3TemplateDialog;
	Ct3TemplateData d_ct3TemplateDataItem;
	LineDetScanWidget* d_lineDetScanWidget;
	std::unique_ptr<LineDetScanInterface> d_airTuneScan;
	int d_objectNumber;
	int d_fileNumber;
	QString d_fileNameComment;					       			
	//文件初始目录，设置路径 + 日期
	QString d_initialFileDirectory;
	QString d_orgDirectory;
public:
	LineDetScanManager(int _rayId, int _lineDetId, 
		const std::vector<ScanMode>& _scanMode, SetupData* _setupData, 
		LineDetNetWork* _lineDetNetWork, ControllerInterface* _controller/*, const QString& _initialDirectory,
		const QString& _orgDirectory, const QString& _objectName, const QString& _objectNumber*/, 
		QWidget *widgetParent = nullptr, QObject *objectParent = nullptr);
	~LineDetScanManager();
	QWidget* getWidget();

private slots:
	void ct3ScanSlot();
	void drScanSlot();
	void updateControlsSlot();
	void stopButtonSlot();
	void LoadCt3TemplateButtonSlot();
	void useCt3ItemSlot();
	void bkgTuneSlot();
	void airTuneSlot();
	void loadTuneDataSlot();
signals:
	void readyToScanSignal(bool _sts);

private:
	void getFileNameFromDialog(QString& _objectName, QString& _objectNumber, QString& _fileNumber, QString& _comment);
};
