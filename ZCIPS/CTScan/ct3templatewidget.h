#pragma once

#include <QDialog>
#include "ui_ct3templatewidget.h"
#include "../Public/util/logmacro.h"
#include "../Public/util/messagebox.h"
#include <vector>
#include <set>

const unsigned char EQUALLAYER = 1;
const unsigned char MULTILAYER = 0;
const unsigned int FILEHEAD = 0XAAAAAAA3;

struct Ct3TemplateData
{
	QString Name;
	unsigned char MutilayerOrEqualLayer;
	unsigned int Matrix;
	unsigned int View;
	unsigned int SampleTime;
	float Orientation;
	unsigned int LayerNumber;  //只用作等层距扫描计数
	float layerSpace;
	std::set<float> LayerPos;
};
bool operator==(const Ct3TemplateData& t1, const Ct3TemplateData& t2);
bool operator<(const Ct3TemplateData& t1, const Ct3TemplateData& t2);


struct CT3Data;
class ct3AddDialog;

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
	bool d_modified;
	int d_lastNameListRow;
	bool loadTemplateData();
	QString d_angleEditText;
	Qt::ItemFlags d_listNameItemFlag;
	std::vector<Ct3TemplateData> d_templateData;
	std::vector<Ct3TemplateData> d_tempTemplateData;
	bool saveTemplateDataToFile();
	std::vector<Ct3TemplateData>::iterator d_currentTempDataIter;
	ct3AddDialog* d_addDialog;
	const CT3Data& d_ct3Data;
	void refresh(int _row);
private slots:
	void on_deleteButton_clicked();
	void on_saveButton_clicked();
	void on_ct3ItemNameListWidget_itemDoubleClicked(QListWidgetItem* _item);
	void on_ct3ItemNameListWidget_currentRowChanged(int _currentRow);
	void on_addButton_clicked();
	//void on_ct3ItemNameListWidget_itemChanged(QListWidgetItem * _item);
protected:
	void closeEvent(QCloseEvent *event) override;
signals:
	LOGSIGNAL
};
