#pragma once

#include <QDialog>
#include "ui_saveconescanconfigdialog.h"
#include "../PanelDetector/panel.h"
//׶��ɨ���ļ��ṹ
//0x0155AA5A   //4�ֽ�
//Item����	 //4�ֽ�
//coneScanDataItem��������

struct coneScanDataItem
{
	QString itemName;
	struct panel
	{
		int gainFactorIndex;
		int cycleTime;
		float re[10];
	};

	int rayIndex;
	int paneldetIndex;
	int graduationComboxIndex;
	int frames;
	float slicePos;
	float orient;
	float sod;
	float sdd;
	float re[10];
	panel panelData;
	QString airName;
};

class SaveConeScanConfigDialog : public QDialog
{
	Q_OBJECT

public:
	SaveConeScanConfigDialog(int rayIndex, int panelDetIndex, float _slicePos, float _orient, 
		float _sod, float _sdd, Panel* _panel, coneScanDataItem *item, QComboBox* d_graduationCombox, int _frames, 
		const QString& _airName, const QString& d_tunedAirDirectory, QWidget *parent = Q_NULLPTR);
	~SaveConeScanConfigDialog();

private:
	Ui::SaveConeScanConfigDialog ui;
	QWidget* d_panelWidget;
	int d_rayIndex;
	int d_panelDetIndex;
	Panel* d_panel;
	coneScanDataItem* d_item;
	QString d_tunedAirDirectory;
	QString d_airName;
private slots:
	void on_okButton_clicked();
	void on_cancelButton_clicked();
	void on_loadAirDataButton_clicked();
};
