#pragma once

#include <QDialog>
#include "ui_AddModifyAirDisposeDialog.h"
#include "../Public/headers/setupdata.h"
#include <memory>
#include <vector>
class LineDetScanInterface;

class AddModifyAirDisposeDialog : public QDialog
{
	Q_OBJECT
public:
	AddModifyAirDisposeDialog(const QString& _orgPath, const QString& _filePath, LineDetScanInterface* _airScan, 
		QWidget *parent = Q_NULLPTR);
	~AddModifyAirDisposeDialog();

private:
	Ui::AddModifyAirDisposeDialog ui;
	LineDetScanInterface* d_airScan;
	QString d_orgPath;
	QString d_filePath;
private slots:
	void on_startButton_clicked();
	void on_stopButton_clicked();
	void on_nameLineEdit_textChanged();
	void on_voltageLineEdit_textChanged();
	void on_ct3LayerThickness_2_textChanged();
	void on_comentEdit_textChanged();
public slots:
	void updateScanButtonSlot(bool _sts);

private:
	static AddModifyAirDisposeDialog* d_ref;

protected:
	virtual void closeEvent(QCloseEvent*_event) override;
};
