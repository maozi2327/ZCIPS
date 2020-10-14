#include "stdafx.h"
#include "linedetairdisposedialog.h"
#include "linedetairtune.h"

const unsigned int FILEHEAD = 0XAAAAAAA0;

LineDetAirDisposeDialog::LineDetAirDisposeDialog(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork,
	const SetupData* _setupData, int _lineDetIndex, int _rayIndex, const QString& _orgPath, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	d_airTuneScan.reset(new LineDetAirTune(_controller,  _lineDetNetWork, _setupData, _lineDetIndex));
	d_airTuneScan->setOrgPath(_orgPath);
}

LineDetAirDisposeDialog::~LineDetAirDisposeDialog()
{
}

struct HeadData
{
	char Name[200];
	float Voltage;
	float Current;
	char Remark[200];
};

bool LineDetAirDisposeDialog::loadTemplateData()
{
	QFile file;
	file.setFileName(d_templateFileName);

	if (!QFile::exists(d_templateFileName))
		LOG_INFO("不存在空气校正配置文件：" + d_templateFileName);

	if (!file.open(QIODevice::ReadOnly))
	{
		LOG_ERROR("打开空气校正配置文件失败：" + d_templateFileName);
		return false;
	}

	unsigned int fileHead = 0;
	file.read((char*)(&fileHead), sizeof(unsigned int));

	if (fileHead != FILEHEAD)
	{
		LOG_ERROR("空气校正配置格式错误：" + d_templateFileName);
		file.close();
		return false;
	}

	unsigned int itemNumber;
	file.read((char*)(&itemNumber), sizeof(unsigned int));

	if (itemNumber > 500)
	{
		LOG_ERROR("空气校正配置数量错误：" + d_templateFileName);
		file.close();
		return false;
	}

	for (int item = 0; item != itemNumber; ++item)
	{
		HeadData data;
		file.read((char*)(&data), sizeof(HeadData));
		AirTemplateData itemData;
		itemData.Name = QString::fromLocal8Bit(data.Name);
		itemData.Voltage = data.Voltage;
		itemData.Current = data.Current;
		itemData.Remark = QString::fromLocal8Bit(data.Remark);
		d_airTemplateData.push_back(itemData);
	}

	file.close();

	if (itemNumber != 0)
	{
		ui.AirDisposeNameListWidget->setCurrentRow(0);
		d_currentTempDataIter = d_airTemplateData.begin();
	}

}
bool LineDetAirDisposeDialog::saveTemplateData()
{
	return true;
}

void LineDetAirDisposeDialog::updateScanButtonSlot(bool _sts)
{

}

void LineDetAirDisposeDialog::on_addButton_clicked()
{
	d_newAirDialog = new AddModifyAirDisposeDialog(d_airTuneScan.get(), d_airTemplateData, 
		d_airTemplateData.end(), this);
	connect(this, &LineDetAirDisposeDialog::updateScanButtonSignal, 
		d_newAirDialog, &AddModifyAirDisposeDialog::updateScanButtonSlot);

	d_newAirDialog->show();
	//if(d_newAirDialog->exec() == QDialog::Accepted)
	//	;
}

void LineDetAirDisposeDialog::on_AirDisposeNameListWidget_currentRowChanged(int _currentRow)
{

}

void LineDetAirDisposeDialog::showEvent(QShowEvent* _event)
{
	loadTemplateData();
}
