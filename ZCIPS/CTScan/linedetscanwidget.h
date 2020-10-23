#pragma once

#include "ui_linedetscanwidget.h"
#include <memory>
#include <vector>
#include <map>
#include "../Public/headers/setupdata.h"
#include "scantemplate.h"

struct CT3Data;
class CT3TemplateDialog;
class LineDetAirDisposeDialog;

class LineDetScanWidget : public QWidget
{
	Q_OBJECT
	friend class LineDetScanManager;
public:
	LineDetScanWidget(QWidget *parent = Q_NULLPTR);
	~LineDetScanWidget();
	void onNetworkStsChanged(bool _netWorkSts);
	void initiliseCt3Controls(const CT3Data& _data);
	void initiliseCt2Controls(const CT2Data& _data);
	void initiliseDrControls(const DrScanData& _data);
	float getLayer();
	void enableScan(bool _sts);
	void setScanButtonToolTip(const QString& _tip);
protected:
#ifndef QT_NO_CONTEXTMENU
	void contextMenuEvent(QContextMenuEvent *event) override;
	//void keyPressEvent(QKeyEvent * _event);
#endif // QT_NO_CONTEXTMENU

private:
	Ui::LineDetScanWidget ui;
	QAction *showMotorTableAction;
	void disableAllControls();
	void initiliseControls();
	void switchCt3MultilayerEquallayerShowHide(unsigned char _mode);
	void useCt3Item(const Ct3TemplateData& _templateData);
	LineDetAirDisposeDialog* d_airDisposeDialog;
private slots:
	void showMotorTable();
	void on_Ct3StartButton_clicked();
	void on_saveDirButton_clicked();
	void on_ct3MultiLayerComboBox_currentIndexChanged(const QString& _text);
	void on_ct3LayerPosLineEdit_returnd();
	void on_stopButton_clicked();
	void on_ct3LoadTemplateButton_clicked();
	void on_airTuneButton_clicked();
	void updateControlsSlot();

public slots:
	void updateCT3Progresser(int _progress);

signals:
	void readyToScanSignal(bool _sts);
	void ct3ScanSignal();
	void stopButtonSignal();
	void LoadCt3TemplateButtonSignal();
};
