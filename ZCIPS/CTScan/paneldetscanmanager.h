#pragma once

#include <QObject>
#include <memory>
#include "ui_conescanwidget.h"
#include "../Public/headers/setupdata.h"
#include "saveconescanconfigdialog.h"

#define IMAGEDIALOGHANDLE void*

class PanelImageProcess;
class ConeScanInterface;
class ControllerInterface;
class Panel;
class ConeScanWidget;
class PanelFrameShot;
class ImageDialogManager;

class PanelDetScanManager : public QObject
{
	Q_OBJECT

public:
	PanelDetScanManager(int _rayId, int _panelDetId, const std::vector<ScanMode>& _scanMode, 
		SetupData* _setupData, Panel* _panel, ControllerInterface* _controller, ImageDialogManager* _imageWidgetManager, 
		const QString& _tunedDirectory, const QString& _orgDirectory, const QString& _tunedBkgDirectory,
		const QString& _tunedAirDirectory, const QString& _tunedConeJointAirDirectory, const QString& _defectFileName, QWidget *widgetParent = nullptr, QObject *objectParent = nullptr);

	~PanelDetScanManager();

private:
	std::unique_ptr<ConeScanInterface> d_scan;
	std::unique_ptr<PanelImageProcess> d_panelImageProcess;
	unsigned short d_rayNum;
	unsigned short d_detNum;
	SetupData* d_setupData;
	ControllerInterface* d_controller;
	Panel* d_panel;
	ConeScanWidget* d_coneScanWidget;
	ImageDialogManager* d_imageWidgetManager;
	std::unique_ptr<PanelFrameShot> d_panelFrameShot;
	virtual void frameShotCallback(unsigned short* _image);
	bool getFileNameFromDialog(QString& _objectName, QString& _objectNumber, QString& _fileNumber, QString& _comment);
	IMAGEDIALOGHANDLE d_imageDialogHandle;
	bool d_loadTunedDataFlag;
	bool d_loadConeJointTunedDataFlag;

	QString d_defectFileName;

	QString d_orgDirectory;
	QString d_tunedFileDirectory;
	QString d_objectName;
	QString d_objectNumber;

	QString d_tunedBkgDirectory;
	QString d_tunedAirDirectory;

	//面阵空气命名规则 空气名 + “_55AA5A_” + 背景名 + “_" + "0/1.tif”
	QString d_tunedConeJointAirDirectory;
	int d_fileNumber;
	QString d_fileNameComment;

	//面阵空气扫描的空气文件名
	QString d_airFileNameA;
	QString d_airFileNameB;

	QString d_airName;
	QString d_bkgName;

	QString d_coneScanConfigFileName;
public:
	QWidget* getWidget();
	void updateFileDirectory(const QString& _orgPath, const QString& _tunedFileDirectory);
	void updateObjectNameNumber(const QString& _objectName, const QString& _objectNumber);
	void updateControlSystemStatus(bool readyToScan);
	void updatePanelStatus(bool readyToScan);
private slots:
	void coneScanBeginSlot();
	void coneJointScanBeginSlot();
	void saveConeScanConfigSlot();
	void loadConeScanConfigSlot();
	//void scanProgressUpdatedSlot();
	void frameShotSlot();
	void coneScanStopSlot();
	void previewSlot();
	void bkgTuneSlot();
	void airTuneSlot();
	void loadTuneDataSlot();
	void loadConeJointScanTuneDataSlot();
	void scanProgressSlot(int _framesAcquiredThisRound, int _framesThisRound, int _framesAcquiredAll, int _framesALL, QString _message);
	void showImageSlot(unsigned short* _image, int _width, int _height);

signals:
	void objectNameNumberChangedSignal(const QString& _objectName, const QString& _objectNumber);
};
