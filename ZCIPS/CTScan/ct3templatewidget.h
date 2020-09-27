#pragma once

#include <QWidget>
#include "ui_ct3templatewidget.h"
#include "../Public/util/logmacro.h"
#include <vector>

struct Ct3TemplateDataRaw
{
	unsigned int itemNum;
	char Name[200];
	unsigned char MutilayerOrEqualLayer;
	unsigned int Matrix;
	unsigned int View;
	float Orientation;
	unsigned int LayerNumber;
	float LayerPos[500];
};

struct Ct3TemplateData
{
	QString Name;
	unsigned char MutilayerOrEqualLayer;
	unsigned int Matrix;
	unsigned int View;
	unsigned int SampleTime;
	float Orientation;
	unsigned int ecqualLayerNumber;  //只用作等层距扫描计数
	unsigned int layerSpace;
	std::vector<float> LayerPos;
};

class CT3Data;

class CT3TemplateWidget : public QWidget
{
	Q_OBJECT

public:
	CT3TemplateWidget(const CT3Data& _ct3Data, QWidget *parent = Q_NULLPTR);
	~CT3TemplateWidget();
private:
	Ui::CT3TemplateWidget ui;
	QString d_templateFileName;
	unsigned short d_rayNum;
	unsigned short d_detNum;
	bool loadTemplateData();
	std::vector<Ct3TemplateData> d_templateData;
	void initiliseCt3Controls(const CT3Data& _data);
	void saveTemplateDataToFile();
private slots:
	void on_saveButton_clicked();
signals:
	LOGSIGNAL
};
