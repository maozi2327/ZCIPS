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

private:
	AddModifyAirDisposeDialog(LineDetScanInterface* _airScan, std::vector<AirTemplateData>& _airTemplateData,
		std::vector<AirTemplateData>::iterator _itr, QWidget *parent = Q_NULLPTR);
public:
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

private:
	static AddModifyAirDisposeDialog* d_ref;

protected:
	virtual void closeEvent(QCloseEvent*_event) override;

public:
	static AddModifyAirDisposeDialog* loadInstance(LineDetScanInterface* _airScan, std::vector<AirTemplateData>& _airTemplateData,
		std::vector<AirTemplateData>::iterator _itr, QWidget *parent = Q_NULLPTR)
	{
		if (d_ref == nullptr)
			d_ref = new AddModifyAirDisposeDialog(_airScan, _airTemplateData, _itr);

		return d_ref;
	}
};
