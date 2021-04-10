#include "stdafx.h"
#include "linedetairtunedialog.h"
#include "linedetairtune.h"
#include <string>
#include <regex>
#include "../Public/util/functions.h"

const unsigned int FILEHEAD = 0XAAAAAAA0;

LineDetAirTuneDialog::LineDetAirTuneDialog(const QString& _orgPath, QWidget *parent)
	: QDialog(parent), d_airFileDirectory(_orgPath)
{
	ui.setupUi(this);
	//TODO_DJ:
	//d_airTuneScan->setFileName(0, 0);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

LineDetAirTuneDialog::~LineDetAirTuneDialog()
{

}

struct HeadData
{
	char Name[200];
	float Voltage;
	float Current;
	char Remark[200];
};

//空气文件命名方式：电压_电流_层厚_备注_文件名.dat
bool LineDetAirTuneDialog::loadFileInfo()
{
	d_airFileDirectory;
	std::vector<QString> fileNames;
	browseAllFiles(d_airFileDirectory, fileNames);

	for (auto& file : fileNames)
	{
		std::regex r(".+(?=\\.dat)");
		std::smatch result;
		std::string fileName = file.toLocal8Bit();
		std::regex_search(fileName, result, r);
		fileName = result.str();
		std::vector<QString> info;
		r = "[^_]+";

		for (std::sregex_iterator it(fileName.begin(), fileName.end(), r), end_it; it != end_it; ++it)
			info.push_back(QString::fromLocal8Bit(it->str().data()));

		ui.AirTuneNameListWidget->addItem(*info.rbegin());
		d_fileInfo.push_back(info);
	}

	return true;

	//QFile file;
	//file.setFileName(d_templateFileName);

	//if (!QFile::exists(d_templateFileName))
	//	LOG_INFO("不存在空气校正配置文件：" + d_templateFileName);

	//if (!file.open(QIODevice::ReadOnly))
	//{
	//	LOG_ERROR("打开空气校正配置文件失败：" + d_templateFileName);
	//	return false;
	//}

	//unsigned int fileHead = 0;
	//file.read((char*)(&fileHead), sizeof(unsigned int));

	//if (fileHead != FILEHEAD)
	//{
	//	LOG_ERROR("空气校正配置格式错误：" + d_templateFileName);
	//	file.close();
	//	return false;
	//}

	//unsigned int itemNumber;
	//file.read((char*)(&itemNumber), sizeof(unsigned int));

	//if (itemNumber > 500)
	//{
	//	LOG_ERROR("空气校正配置数量错误：" + d_templateFileName);
	//	file.close();
	//	return false;
	//}

	//for (int item = 0; item != itemNumber; ++item)
	//{
	//	HeadData data;
	//	file.read((char*)(&data), sizeof(HeadData));
	//	AirTemplateData itemData;
	//	itemData.Name = QString::fromLocal8Bit(data.Name);
	//	itemData.Voltage = data.Voltage;
	//	itemData.Current = data.Current;
	//	itemData.Remark = QString::fromLocal8Bit(data.Remark);
	//	d_airTemplateData.push_back(itemData);
	//}

	//file.close();

	//if (itemNumber != 0)
	//	ui.AirTuneNameListWidget->setCurrentRow(0);
}
bool LineDetAirTuneDialog::saveTemplateData()
{
	return true;
}

void LineDetAirTuneDialog::updateScanButtonSlot(bool _sts)
{
	emit updateScanButtonSignal(_sts);
}

void LineDetAirTuneDialog::on_addButton_clicked()
{

}

void LineDetAirTuneDialog::on_AirTuneNameListWidget_currentRowChanged(int _currentRow)
{
	ui.voltageLabel->setText(d_fileInfo[_currentRow][0]);
	ui.currentLabel->setText(d_fileInfo[_currentRow][1]);
	ui.layerThicknessLabel->setText(d_fileInfo[_currentRow][2]);
	ui.comentLabel->setText(d_fileInfo[_currentRow][3]);
}

void LineDetAirTuneDialog::showEvent(QShowEvent* _event)
{
	loadFileInfo();
}

void LineDetAirTuneDialog::closeEvent(QCloseEvent* _event)
{
	if (d_newAirDialog != nullptr)
	{
		int ret = messageBox(QString::fromLocal8Bit("未关闭新建校正对话框"),
			QString::fromLocal8Bit("请关闭新建对话框"));
		_event->ignore();
		return;
	}
	//else
	//{
	//	switch (ret)
	//	{
	//	case QMessageBox::Save:
	//		if (save())
	//		{
	//			_event->accept();
	//			accept();
	//		}
	//		else
	//			event->ignore();

	//		break;
	//	case QMessageBox::Discard:
	//		event->accept();
	//		reject();
	//		break;
	//}
}
