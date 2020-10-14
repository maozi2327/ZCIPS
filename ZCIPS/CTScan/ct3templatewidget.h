#pragma once

#include <QDialog>
#include "ui_ct3templatewidget.h"
#include "../Public/util/logmacro.h"
#include "../Public/util/messagebox.h"
#include <vector>
#include <set>
#include "scantemplate.h"

struct CT3Data;
class ct3AddDialog;

class CT3TemplateWidget : public QDialog
{
	Q_OBJECT

public:
	CT3TemplateWidget(const CT3Data& _ct3Data, Ct3TemplateData& _templateData, QWidget *parent = Q_NULLPTR);
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
	Ct3TemplateData& d_templateDataUseItem;
	ct3AddDialog* d_addDialog;
	const CT3Data& d_ct3Data;
	void refresh(int _row);

private slots:
	void on_saveButton_clicked();
	void on_ct3ItemNameListWidget_itemDoubleClicked(QListWidgetItem* _item);
	void on_ct3ItemNameListWidget_currentRowChanged(int _currentRow);
	void on_addButton_clicked();
	void on_useButton_clicked();
	void listNameContextMenuSlot(const QPoint&);
	void deleteListNameItemSlot();
	//void on_ct3ItemNameListWidget_itemChanged(QListWidgetItem * _item);

protected:
	void closeEvent(QCloseEvent *event) override;

signals:
	void useItemSignal();
	LOGSIGNAL

};
