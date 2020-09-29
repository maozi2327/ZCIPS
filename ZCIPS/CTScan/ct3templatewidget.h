#pragma once

#include <QDialog>
#include "ui_ct3templatewidget.h"
#include "../Public/util/logmacro.h"
#include <vector>
#include <set>

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
	std::set<float> LayerPos;
};

struct CT3Data;

class CT3TemplateWidget : public QDialog
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
	bool d_saved;
	int d_lastNameListRow;
	bool loadTemplateData();
	QString d_angleEditText;
	QString d_layerNumberEditText;
	std::vector<Ct3TemplateData> d_templateData;
	std::vector<Ct3TemplateData> d_tempTemplateData;
	void initiliseCt3Controls(const CT3Data& _data);
	void saveTemplateDataToFile();
	void refreshPosListWidget();
	void saveItem();
	std::vector<Ct3TemplateData>::iterator d_currentTempDataIter;
private slots:
	void on_deleteButton_clicked();
	void on_saveButton_clicked();
	void on_ct3MultiLayerComboBox_currentIndexChanged(const QString& _text);
	void on_ct3LayerPosEdit_returnd();
	void on_ct3LayerPosListWidget_itemDoubleClicked(QListWidgetItem* _item);
	void on_ct3ItemNameListWidget_currentRowChanged(int _currentRow);
	void on_ct3ItemNameListWidget_itemDoubleClicked(QListWidgetItem * _item);
	void on_ct3MatrixComboBox_currentIndexChanged(const QString& _text);
	void on_ct3ViewComboBox_currentIndexChanged(const QString& _text);
	void on_ct3SampleTimeComboBox_currentIndexChanged(const QString& _text);
public slots:
	bool eventFilter(QObject* _object, QEvent* _event);
signals:
	LOGSIGNAL
};
