#pragma once

#include <QObject>
#include "imagewidgetmanager.h"
#include <memory>
#include "../Public/headers/setupdata.h"
#include "../Public/util/logmacro.h"
#include "ctscanexport.h"

class SetupDataParser;
class RayPanelMotion;
class MotorControlWidget;
class LineDetScanWidget;
class ControllerInterface;
class LineDetNetWork;
class MsgListBoxDialog;
class LineDetScanInterface;
class ConeScanWidget;
class MotorWidget;
class CTScanWidget;

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
	void ctScanWidgetClosedSlot();
protected:

private:
	std::unique_ptr<SetupData> d_setupData;
	std::unique_ptr<SetupDataParser> d_setupDataPaser;
	std::unique_ptr<ImageWidgetManager> d_imageWidgetManager;
	std::unique_ptr<ControllerInterface> d_controller;
	MotorWidget* d_motorWidget;
	std::unique_ptr<ConeScanWidget> d_panle1Det1ScanWidget;
	//std::unique_ptr<MsgListBoxDialog> d_msg;			//消息对话框无父窗口
	MsgListBoxDialog* d_msg;			//消息对话框无父窗口
	std::map<int, std::unique_ptr<LineDetNetWork>> d_lineDetNetWorkMap;
	std::map<std::pair<int, int>, std::vector<ScanMode>> d_lineDetScanModeMap;
	std::map<std::pair<int, int>, std::vector<ScanMode>> d_panelDetScanModeMap;
	std::map<std::pair<int, int>, LineDetScanWidget*> d_lineDetScanWidget;
	std::map<std::pair<int, int>, ConeScanWidget*> d_panelDetScanWidget;
	CTScanWidget* d_mainWidget;

	size_t frontImageIndex;
	QSystemTrayIcon* d_tray;
	QString	d_workDir;
	QWidget* d_upperWidget;
public:
	void setMiddleWidget(QWidget* _widget);
	void setDownWidget(QWidget* _widget);
};
