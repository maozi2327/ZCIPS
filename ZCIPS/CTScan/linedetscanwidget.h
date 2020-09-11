#pragma once

#include "ui_linedetscanwidget.h"
#include <memory>
#include <vector>
#include <map>
#include "../Public/headers/setupdata.h"

struct CT3Data;
class MotorControlWidget;
class LineDetScanInterface;
class ControllerInterface;
class LineDetNetWork;
class LineDetScanWidget : public QWidget
{
	Q_OBJECT

public:
	LineDetScanWidget(int _rayId, int _lineDetId, const	std::vector<ScanMode>& _scanMode, SetupData* _setupData, 
		LineDetNetWork* _lineDetNetWork, ControllerInterface* _controller, QWidget *parent = Q_NULLPTR);

	~LineDetScanWidget();

	void onNetworkStsChanged(bool _netWorkSts);
protected:
#ifndef QT_NO_CONTEXTMENU
	void contextMenuEvent(QContextMenuEvent *event) override;

	//void keyPressEvent(QKeyEvent * _event);
#endif // QT_NO_CONTEXTMENU
private:
	Ui::LineDetScanWidget ui;
	QAction *showMotorTableAction;
	MotorControlWidget* d_motorControlDialog;
	SetupData* d_setupData;
	LineDetNetWork* d_lineDetNetWork;
	ControllerInterface* d_controller;
	QTimer* d_timer;
	unsigned short d_rayNum;
	unsigned short d_detNum;
	void disableAllControls();
	void initiliseControls();
	void initiliseCt3Controls(CT3Data& _data);
	void initiliseCt2Controls(CT2Data& _data);
	void initiliseDrControls(DrScanData& _data);
	std::map<ScanMode, LineDetScanInterface*> d_scanMap;
	std::unique_ptr<LineDetScanInterface> d_scan;
private slots:
	void showMotorTable();
	void on_Ct3StartButton_clicked();
	void on_saveDirButton_clicked();
	void on_ct3MultiLayerComboBox_currentIndexChanged(const QString& _text);
	void on_ct3LayerPosLineEdit_returnd();
	void on_stopButton_clicked();
	void updateControlsSlot();
};
