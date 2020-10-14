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
	AddModifyAirDisposeDialog(LineDetScanInterface* _airScan, std::vector<AirTemplateData>& _airTemplateData,
		std::vector<AirTemplateData>::iterator _itr, QWidget *parent = Q_NULLPTR);
	~AddModifyAirDisposeDialog();

private:
	Ui::AddModifyAirDisposeDialog ui;
	LineDetScanInterface* d_airScan;
	std::vector<AirTemplateData>& d_airTemplateData;
	std::vector<AirTemplateData>::iterator d_itr;
private slots:
	void on_startButton_clicked();
	void on_stopButton_clicked();

public slots:
	void updateScanButtonSlot(bool _sts);
};
