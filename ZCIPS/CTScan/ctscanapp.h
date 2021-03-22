#pragma once

#include <QObject>
#include <memory>
#include "../Public/headers/setupdata.h"
#include "../Public/util/logmacro.h"
#include "ctscanexport.h"
#include "applicationSettingData.h"

class SetupDataParser;
class RayPanelMotion;
class LineDetScanWidget;
class ControllerInterface;
class LineDetNetWork;
class Panel;
class MsgListBoxDialog;
class LineDetScanInterface;
class ConeScanWidget;
class CTScanWidget;
class AcceleratorWidget;
class CT3TemplateDialog;
class LineDetScanManager;
class PanelDetScanManager;
class LineDetImageProcess;
class ImageDialogManager;
class AxisStatusWidget;
class AxisZeroCoordinationDialog;
class AxisStatusDialog;
class FilePathSettingDialog;

class CTScanApp : public CTScanInterface
{
	Q_OBJECT
public:
	CTScanApp(QWidget* d_upper, QObject *parent = nullptr);
	~CTScanApp();

signals:
	LOGSIGNAL;
private slots:
	void controllerNetWorkStsSlot(bool sts);
	void lineDetNetWorkStsSlot(int _det, bool sts);
	void errorMsgSlot(QString msg);
	void infoMsgSlot(QString msg);
	void bugMsgSlot(QString msg);
	void motorButonSlot();
	void switchToPanelWidgetSlot(int _rayId, int _detId);
	void switchToLineWidgetSlot(int _rayId, int _detId);
protected:

private:
	std::unique_ptr<SetupData> d_setupData;
	std::unique_ptr<ConfigData> d_configData;
	std::unique_ptr<SetupDataParser> d_setupDataPaser;
	std::unique_ptr<ImageDialogManager> d_imageWidgetManager;
	std::unique_ptr<ControllerInterface> d_controller;
	std::unique_ptr<ConeScanWidget> d_panle1Det1ScanWidget;
	std::unique_ptr<LineDetImageProcess> d_lineDetImageProcess;
	//std::unique_ptr<MsgListBoxDialog> d_msg;			//消息对话框无父窗口
	std::map<int, std::unique_ptr<LineDetNetWork>> d_lineDetNetWorkMap;
	std::map<int, std::unique_ptr<Panel>> d_panelDetMap;
	std::map<std::pair<int, int>, std::vector<ScanMode>> d_lineDetScanModeMap;
	std::map<std::pair<int, int>, std::vector<ScanMode>> d_panelDetScanModeMap;
	std::map<std::pair<int, int>, LineDetScanManager*> d_lineDetScanManagerMap;
	std::map<std::pair<int, int>, PanelDetScanManager*> d_panelDetScanManagerMap;

	ApplicationSettingData d_appSettingData;
	const QString d_appSettingDataFileName{QString::fromLocal8Bit("appSettingData.data")};
	std::map<Axis, AxisData> d_axisDataMap;
	MsgListBoxDialog* d_msg;			//消息对话框无父窗口
	CTScanWidget* d_mainWidget;
	AcceleratorWidget* d_acceleratorWidget;
	QDialog* d_floatAcceleratorDialog;
	size_t frontImageIndex;
	QSystemTrayIcon* d_tray;
	QString	d_workDir;
	QWidget* d_upperWidget;
	AxisStatusWidget* d_axisStatusWidget;
	AxisZeroCoordinationDialog* d_axisZeroCoordinationDialog = nullptr;
	AxisStatusDialog* d_axisStatusDialog = nullptr;
	bool d_debugSystem = true;

	QMenuBar* d_menuBar;
	QMenu* d_systemMenu;
	QAction* d_initiliseSystemAction;
	QAction* d_filePathSettingAction;
	QMenu* d_rayDetSwitchMenu;
	std::map<std::pair<int, int>, QAction*> d_lineDetRayActions;
	std::map<std::pair<int, int>, QAction*> d_panelDetRayActions;

	QMenu* d_viewMenu;
	QAction* d_axisStatusAction;
	QMenu* d_debugMenu;
	QAction* d_axisZeroCoordinateAction;
	QAction* d_axisSpeedAction; 
	QAction* d_lineDetectorAction;
	QAction* d_autoAlignLayerAction;
	QAction* d_laserInterferometerAction;
	
	QString d_orgPath;
	QString d_disposedFilePath;
public:
	void setMiddleWidget(QWidget* _widget);
	void setDownWidget(QWidget* _widget);
	void ctScanWidgetClosed();
	void switchToLineWidget(int _rayId, int _detId);
	void switchToPanelWidget(int _rayId, int _detId);

	virtual QMenuBar* getMenuBar();
private:
	void buildMenuBar();
	bool saveAppSettingData();
	bool getAppSettingDataFromFile();
private slots:
	void on_initiliseSystemAction_triggered();
	void on_filePathSettingAction_triggered();
	void on_rayDetAction_triggered();
	void on_axisStatusAction_triggered();
	void on_axisZeroCoordinateAction_triggered();
	void on_axisSpeedAction_triggered();
	void on_lineDetectorAction_triggered();
	void on_autoAlignLayerAction_triggered();
	void on_laserInterferometerAction_triggered();
};
