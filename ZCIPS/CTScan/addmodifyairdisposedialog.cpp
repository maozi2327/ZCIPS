#include "stdafx.h"
#include "addmodifyairdisposedialog.h"
#include "LineDetAirTune.h"
#include "linedetairdisposedialog.h"
#include "../Public/util/messagebox.h"

AddModifyAirDisposeDialog::AddModifyAirDisposeDialog(LineDetScanInterface* _airScan, std::vector<AirTemplateData>& _airTemplateData, 
	std::vector<AirTemplateData>::iterator _itr, QWidget *parent)
	: d_airScan(_airScan), d_airTemplateData(_airTemplateData), d_itr(_itr), QDialog(parent)
{
	ui.setupUi(this);
}

AddModifyAirDisposeDialog::~AddModifyAirDisposeDialog()
{
}


void AddModifyAirDisposeDialog::on_startButton_clicked()
{
	auto name = ui.nameLineEdit->text();

	if (d_itr == d_airTemplateData.end())
	{
		if (std::find_if(d_airTemplateData.begin(), d_airTemplateData.end(),
			[&name](AirTemplateData& item) { return item.Name == name; }) != d_airTemplateData.end())
		{
			messageBox(QString::fromLocal8Bit("�Ѿ�����ͬ����Ŀ"), QString::fromLocal8Bit("����������"));
			return;
		}
		else
			d_airScan->setFileName(name);
	}

	d_airScan->beginScan();
}

void AddModifyAirDisposeDialog::on_stopButton_clicked()
{
	d_airScan->stopScan();
}

void AddModifyAirDisposeDialog::updateScanButtonSlot(bool _sts)
{
	ui.startButton->setEnabled(_sts);
}