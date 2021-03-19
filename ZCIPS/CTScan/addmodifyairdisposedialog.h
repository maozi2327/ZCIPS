#pragma once

#include <QDialog>
#include "ui_AddModifyAirDisposeDialog.h"
#include "../Public/headers/setupdata.h"
#include <memory>
#include <vector>
class LineDetScanInterface;
struct AirTemplateData;

class AddModifyAirDisposeDialog : public QDialog
{
	Q_OBJECT
public:
	AddModifyAirDisposeDialog(LineDetScanInterface* _airScan, QWidget *parent = Q_NULLPTR);
	~AddModifyAirDisposeDialog();

private:
	Ui::AddModifyAirDisposeDialog ui;
	LineDetScanInterface* d_airScan;
private slots:
	void on_startButton_clicked();
	void on_stopButton_clicked();

public slots:
	void updateScanButtonSlot(bool _sts);

private:
	static AddModifyAirDisposeDialog* d_ref;

protected:
	virtual void closeEvent(QCloseEvent*_event) override;
};
