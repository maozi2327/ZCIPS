#pragma once

#include <QObject>
#include <memory>
#include "ui_conescanwidget.h"
#include "../Public/headers/setupdata.h"

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
		QWidget *widgetParent = nullptr, QObject *objectParent = nullptr);
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

public:
	QWidget* getWidget();

private slots:
	void coneScanBeginSlot();
	void scanProgressUpdatedSlot();
	void frameShotSlot();
	void stopControllerSlot();
	void scanProgressSlot(float _progress);
	void showImageSlot(unsigned short* _image, int _width, int _height);
};
