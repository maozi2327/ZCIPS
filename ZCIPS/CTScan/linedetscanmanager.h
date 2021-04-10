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
class LineDetAirTuneDialog;
class NewLineDetAirTuneDialog;
class LineDetScanManager;
class LineDetScanWidget;
class LinedetFileNameDialog;
class LineDetAirTune;
class LineDetImageProcess;
class NewLineDetBkgTuneDialog;

class LineDetScanManager : public QObject
{
	Q_OBJECT
private:
	SetupData* d_setupData;
	ConfigData* d_configData;
	LineDetNetWork* d_lineDetNetWork;
	ControllerInterface* d_controller;
	LineDetImageProcess* d_lineDetImageProcess;
	QTimer* d_timer;
	unsigned short d_rayNum;
	unsigned short d_detNum;
	std::map<ScanMode, LineDetScanInterface*> d_scanMap;
	std::unique_ptr<LineDetScanInterface> d_scan;
	CT3Data d_ct3Data;
	LineDetAirTuneDialog* d_airTuneDialog;
	NewLineDetAirTuneDialog* d_newLineDetAirTuneDialog;
	NewLineDetBkgTuneDialog* d_newLineDetBkgTuneDialog;
	CT3TemplateDialog* d_ct3TemplateDialog;
	Ct3TemplateData d_ct3TemplateDataItem;
	LineDetScanWidget* d_lineDetScanWidget;
	std::unique_ptr<LineDetScanInterface> d_airTuneScan;
	int d_fileNumber;
	QString d_fileNameComment;					       			
	//文件初始目录，设置路径 + 日期
	QString d_tunedFileDirectory;
	QString d_orgDirectory;
	QString d_objectName;
	QString d_objectNumber;
public:
	LineDetScanManager(int _rayId, int _lineDetId, 
		const std::vector<ScanMode>& _scanMode, SetupData* _setupData, 
		LineDetNetWork* _lineDetNetWork, ControllerInterface* _controller, LineDetImageProcess* _lineDetImageProcess,
		const QString& _tunedDirectory, const QString& _orgDirectory/*, const QString& _objectName, const QString& _objectNumber*/, 
		QWidget *widgetParent = nullptr, QObject *objectParent = nullptr);
	~LineDetScanManager();
	QWidget* getWidget();
	void updateFileDirectory(const QString& _orgPath, const QString& _tunedFileDirectory);
	void updateStatus(bool readyToScan);
	void updateObjectNameNumber(const QString& _objectName, const QString& _objectNumber);

private slots:
	void ct3ScanSlot();
	void drScanSlot();
	void stopButtonSlot();
	void LoadCt3TemplateButtonSlot();
	void useCt3ItemSlot();
	void bkgTuneSlot();
	void airTuneSlot();
	void loadTuneDataSlot();
signals:
	void readyToScanSignal(bool _sts);
	void objectNameNumberChangedSignal(const QString& _objectName, const QString& _objectNumber);

private:
	bool getFileNameFromDialog(QString& _objectName, QString& _objectNumber, QString& _fileNumber, QString& _comment);
};
