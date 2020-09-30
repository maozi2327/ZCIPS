#pragma once

#include <QDialog>
#include "ui_ct3AddModifyDialog.h"
struct Ct3TemplateData;
struct CT3Data;

class ct3AddModifyDialog : public QDialog
{
	Q_OBJECT

public:
	ct3AddModifyDialog(std::vector<Ct3TemplateData>& _tempData,
		std::vector<Ct3TemplateData>::iterator _itr, const CT3Data& _ct3Data, QWidget *parent = Q_NULLPTR);
	~ct3AddModifyDialog();

private:
	Ui::ct3AddModifyDialog ui;
	void initiliseCt3Controls(const CT3Data & _data);
	std::vector<Ct3TemplateData>::iterator d_itr;
	QString d_angleEditText;
	void refreshPosListWidget();

private slots:
	void on_ct3MatrixComboBox_currentIndexChanged(const QString& _text);
	void on_ct3ViewComboBox_currentIndexChanged(const QString& _text);
	void on_ct3SampleTimeComboBox_currentIndexChanged(const QString& _text);
	void on_okButton_Clicked();
	void on_ct3LayerPosEdit_returnPressed();
	void on_ct3LayerPosListWidget_itemDoubleClicked(QListWidgetItem * _item);
	void on_ct3MultiLayerComboBox_currentIndexChanged(const QString& _text);
public slots:
	bool eventFilter(QObject* _object, QEvent* _event);
};
