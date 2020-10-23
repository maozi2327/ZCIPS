#pragma once

#include <QObject>
#include "imagewidgetmanager.h"
#include <memory>
#include "../Public/headers/setupdata.h"
#include "../Public/util/logmacro.h"
#include "ctscanexport.h"

class SetupDataParser;
class RayPanelMotion;
class LineDetScanWidget;
class ControllerInterface;
class LineDetNetWork;
class Panel;
class MsgListBoxDialog;
class LineDetScanInterface;
class ConeScanWidget;
class MotorWidget;
class CTScanWidget;
class AcceleratorWidget;
class CT3TemplateDialog;
class LineDetScanManager;
class PanelDetScanManager;
class LineDetImageProcess;

class CTScanApp : public CTScanInterface
{
	Q_OBJECT
public:
	CTScanApp(QWidget* d_upper, QObject *parent = nullptr);
	~CTScanApp();

signals:
	LOGSIGNAL
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
	std::unique_ptr<ImageWidgetManager> d_imageWidgetManager;
	std::unique_ptr<ControllerInterface> d_controller;
	std::unique_ptr<ConeScanWidget> d_panle1Det1ScanWidget;
	std::unique_ptr<LineDetImageProcess> d_lineDetImageProcess;
	//std::unique_ptr<MsgListBoxDialog> d_msg;			//��Ϣ�Ի����޸�����
	std::map<int, std::unique_ptr<LineDetNetWork>> d_lineDetNetWorkMap;
	std::map<int, std::unique_ptr<Panel>> d_panelDetMap;
	std::map<std::pair<int, int>, std::vector<ScanMode>> d_lineDetScanModeMap;
	std::map<std::pair<int, int>, std::vector<ScanMode>> d_panelDetScanModeMap;
	std::map<std::pair<int, int>, LineDetScanManager*> d_lineDetScanManagerMap;
	std::map<std::pair<int, int>, PanelDetScanManager*> d_panelDetScanManagerMap;
	MsgListBoxDialog* d_msg;			//��Ϣ�Ի����޸�����
	MotorWidget* d_motorWidget;
	CTScanWidget* d_mainWidget;
	AcceleratorWidget* d_acceleratorWidget;
	QDialog* d_floatAcceleratorDialog;
	size_t frontImageIndex;
	QSystemTrayIcon* d_tray;
	QString	d_workDir;
	QWidget* d_upperWidget;

public:
	void setMiddleWidget(QWidget* _widget);
	void setDownWidget(QWidget* _widget);
	void ctScanWidgetClosed();
	void switchToLineWidget(int _rayId, int _detId);
	void switchToPanelWidget(int _rayId, int _detId);
};
