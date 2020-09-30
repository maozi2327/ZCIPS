#include "stdafx.h"
#include "ct3templatewidget.h"
#include "../Public/headers/SetupData.h"
#include "ct3AddModifyDialog.h"

bool operator==(const Ct3TemplateData& t1, const Ct3TemplateData& t2)
{
	bool flag1 =
		t1.Name == t2.Name &&
		t1.MutilayerOrEqualLayer == t2.MutilayerOrEqualLayer &&
		t1.Matrix == t2.Matrix &&
		t1.View == t2.View &&
		t1.SampleTime == t2.SampleTime &&
		t1.Orientation == t2.Orientation;

	bool flag2;

	if (t1.MutilayerOrEqualLayer == EQUALLAYER)
		flag2 =
		t1.ecqualLayerNumber == t2.ecqualLayerNumber &&
		t1.layerSpace == t2.layerSpace &&
		t1.LayerPos == t2.LayerPos;
	else if (t1.MutilayerOrEqualLayer == EQUALLAYER)
		flag2 = t1.LayerPos == t2.LayerPos;

	return flag1 && flag2;
}

int messageBox(const QString& text, const QString& infoText)
{
	QMessageBox msgBox;
	msgBox.setText(text);
	msgBox.setInformativeText(infoText);
	msgBox.setStandardButtons(QMessageBox::Ok);
	return msgBox.exec();
}

int messageBoxOkCancel(const QString& text, const QString& infoText)
{
	QMessageBox msgBox;
	msgBox.setText(text);
	msgBox.setInformativeText(infoText);
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	return msgBox.exec();
}

int messageBoxOkCancelDiscard(const QString& text, const QString& infoText)
{
	QMessageBox msgBox;
	msgBox.setText(text);
	msgBox.setInformativeText(infoText);
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Discard);
	return msgBox.exec();
}

CT3TemplateWidget::CT3TemplateWidget(const CT3Data& _ct3Data, QWidget *parent)
	: QDialog(parent), d_ct3Data(_ct3Data)
{
	ui.setupUi(this);
	ui.ct3MultiLayerComboBox->setEnabled(false);
	ui.ct3LayerPosListWidget->setEnabled(false);
	ui.ct3LayerSpaceLineEdit->setEnabled(false);
	ui.ct3LayerNumLineEdit->setEnabled(false);
	ui.ct3LayerPosEdit->setEnabled(false);
	ui.ct3MatrixComboBox->setEnabled(false);
	ui.ct3ViewComboBox->setEnabled(false);
	ui.ct3SampleTimeComboBox->setEnabled(false);
	ui.ct3AngleLineEdit->setEnabled(false);
	loadTemplateData();
	QString str;
	d_templateFileName = str.sprintf("%d_%d_ct3Template", _ct3Data.Ray, _ct3Data.Det);
	d_saved = true;
}

CT3TemplateWidget::~CT3TemplateWidget()
{

}

struct HeadData
{
	char Name[200];
	unsigned char MutilayerOrEqualLayer;
	unsigned int Matrix;
	unsigned int View;
	unsigned int SampleTime;
	float Orientation;
	unsigned int LayerNumber;
	float LayerPos[];
};

bool CT3TemplateWidget::loadTemplateData()
{
	d_currentTempDataIter = d_tempTemplateData.end();
	QFile file;
	file.setFileName(d_templateFileName);

	if (!QFile::exists(d_templateFileName))
		LOG_INFO("不存在配置文件：" + d_templateFileName);

	if (!file.open(QIODevice::ReadOnly))
	{
		LOG_ERROR("打开三代CT配置文件失败：" + d_templateFileName);
		return false;
	}

	unsigned int fileHead = 0;
	file.read((char*)(&fileHead), sizeof(unsigned int));

	if(fileHead != FILEHEAD)
	{
		LOG_ERROR("三代CT配置文件格式错误：" + d_templateFileName);
		return false;
	}

	unsigned int itemNumber;
	file.read((char*)(&itemNumber), sizeof(unsigned int));
	
	if(itemNumber > 500)
	{
		LOG_ERROR("三代CT配置文件配置项数量错误：" + d_templateFileName);
		return false;
	}

	for (int item = 0; item != itemNumber; ++item)
	{
		HeadData data;
		file.read((char*)(&data), sizeof(HeadData));
		Ct3TemplateData itemData;
		itemData.Name = QString::fromLocal8Bit(data.Name);
		QListWidgetItem* listItem = new QListWidgetItem(itemData.Name);
		ui.ct3ItemNameListWidget->addItem(listItem);
		memcpy(&itemData.MutilayerOrEqualLayer,
			&data.MutilayerOrEqualLayer,
			(char*)(&data.LayerNumber) - (char*)(&data.MutilayerOrEqualLayer));

		for (int i = 0; i != data.LayerNumber; ++i)
		{	
			float pos;
			file.read((char*)(&pos), sizeof(float));
			itemData.LayerPos.insert(pos);
		}

		d_tempTemplateData.push_back(itemData);
	}

	d_templateData = d_tempTemplateData;
	file.flush();
	file.close();

	if (itemNumber != 0) {
		ui.ct3ItemNameListWidget->setCurrentRow(0);
		d_currentTempDataIter = d_tempTemplateData.begin();
	}
		
	return true;
}

void CT3TemplateWidget::saveTemplateDataToFile()
{
	size_t fileSize = sizeof(unsigned int) * 2;
	std::vector<std::pair<char*, size_t>> dataVec;

	for (auto& item : d_tempTemplateData)
	{
		auto layerNumber = item.LayerPos.size();
		auto dataSize = sizeof(HeadData) + layerNumber * sizeof(float);
		HeadData* data = (HeadData*)malloc(dataSize);
		data->LayerNumber = layerNumber;
		QByteArray rawByteName = item.Name.toLocal8Bit();
		memcpy(data->Name, item.Name.toLocal8Bit().data(), rawByteName.size());
		data->Name[rawByteName.size()] = '\0';
		memcpy(&data->MutilayerOrEqualLayer,
			&item.MutilayerOrEqualLayer,
			(char*)(&data->LayerNumber) - (char*)(&data->MutilayerOrEqualLayer));
		std::copy(item.LayerPos.begin(), item.LayerPos.end(), data->LayerPos);
		fileSize += dataSize;
		dataVec.push_back({ (char*)data, dataSize });
	}

	unsigned int* fileMem = (unsigned int*)malloc(fileSize);
	*fileMem = FILEHEAD;
	auto ptr = fileMem + 1;
	*ptr = d_tempTemplateData.size();
	char* fileDataHead = (char*)fileMem + 2 * sizeof(unsigned int);
	size_t pos = 0;

	for (auto item : dataVec)
	{
		memmove(fileDataHead + pos, item.first, item.second);
		pos += item.second;
	}

	if (!QFile::exists(d_templateFileName))
		LOG_INFO("不存在配置文件：" + d_templateFileName + "，已经自动创建配置文件");

	QFile file;
	file.setFileName(d_templateFileName);

	if (!file.open(QIODevice::WriteOnly))
	{
		LOG_ERROR("打开三代CT配置失败：" + d_templateFileName);
		free(fileMem);
		return;
	}

	file.write((char*)(fileMem), fileSize);
	free(fileMem);
	file.flush();
	file.close();
	d_templateData = d_tempTemplateData;
	d_saved = true;
}

void CT3TemplateWidget::on_deleteButton_clicked()
{
	int row = ui.ct3ItemNameListWidget->currentRow();
	QListWidgetItem* item = ui.ct3ItemNameListWidget->takeItem(row);
	delete item;
	d_tempTemplateData.erase(d_tempTemplateData.begin() + row);
}

void CT3TemplateWidget::on_saveButton_clicked()
{
	if (d_currentTempDataIter->MutilayerOrEqualLayer == MULTILAYER)
	{
		if (d_currentTempDataIter->LayerPos.size() == 0)
		{
			messageBox(QString::fromLocal8Bit("参数不完整"),
				QString::fromLocal8Bit("请输入正确参数"));
			ui.ct3ItemNameListWidget->setCurrentRow(d_lastNameListRow);
			return;
		}
	}
	else if (d_currentTempDataIter->MutilayerOrEqualLayer == EQUALLAYER)
	{
		if (d_currentTempDataIter->LayerPos.size() == 0 |
			d_currentTempDataIter->ecqualLayerNumber == 0 |
			d_currentTempDataIter->layerSpace == 0)
		{
			messageBox(QString::fromLocal8Bit("参数不完整"),
				QString::fromLocal8Bit("请输入正确参数"));
			ui.ct3ItemNameListWidget->setCurrentRow(d_lastNameListRow);
			return;
		}
	}

	saveTemplateDataToFile();
}

void CT3TemplateWidget::on_ct3ItemNameListWidget_itemDoubleClicked(QListWidgetItem * _item)
{
	auto row = ui.ct3ItemNameListWidget->row(_item);
	ct3AddModifyDialog* dlg = new ct3AddModifyDialog(d_tempTemplateData, d_tempTemplateData.begin() + row, d_ct3Data, this);
	dlg->show();
}

void CT3TemplateWidget::on_ct3ItemNameListWidget_currentRowChanged(int _currentRow)
{
	if (d_tempTemplateData.size() == 0)
		return;
	
	d_lastNameListRow = _currentRow;
	Ct3TemplateData dataItem = d_tempTemplateData[_currentRow];

	if (dataItem.MutilayerOrEqualLayer == MULTILAYER)
	{
		ui.ct3MultiLayerComboBox->setCurrentText(QString::fromLocal8Bit("多层不等间距"));
		ui.ct3LayerPosListWidget->clear();

		for (auto pos : dataItem.LayerPos)
		{
			QListWidgetItem* item = new QListWidgetItem(QString::number(pos, 'f', 2));
			ui.ct3LayerPosListWidget->addItem(item);
		}
	}
	else if (dataItem.MutilayerOrEqualLayer == EQUALLAYER)
	{
		ui.ct3MultiLayerComboBox->setCurrentText(QString::fromLocal8Bit("多层等间距"));
		ui.ct3LayerSpaceLineEdit->setText(QString::number(dataItem.layerSpace, 'f', 2));
		ui.ct3LayerNumLineEdit->setText(QString::number(dataItem.ecqualLayerNumber));
		ui.ct3LayerPosEdit->setText(QString::number(*dataItem.LayerPos.begin()));
	}

	ui.ct3MatrixComboBox->setCurrentText(QString::number(dataItem.Matrix));
	ui.ct3ViewComboBox->setCurrentText(QString::number(dataItem.View));
	ui.ct3SampleTimeComboBox->setCurrentText(QString::number(dataItem.SampleTime));
	ui.ct3AngleLineEdit->setText(QString::number(dataItem.Orientation, 'f', 2));
}
//void CT3TemplateWidget::on_ct3ItemNameListWidget_itemChanged(QListWidgetItem * _item)
//{
//	//首先在列表添加项目，然后保存到配置向量中，最后从向量读出数据保存到文件
//	QString itemName = ui.templateNameEdit->text();
////	if (itemName.size() == 0)
//	{
//		LOG_ERROR("配置项命名不能为空：" + d_templateFileName);
//		return;
//	}
////	auto itr = std::find_if(d_tempTemplateData.begin(), d_tempTemplateData.end(),
//		[&itemName](Ct3TemplateData& item) { return item.Name == itemName; });
//
//	if (itr != d_tempTemplateData.end())
//	{
//		LOG_INFO("已存在同名配置项！");
//		return;
//	}
////	Ct3TemplateData item;
//	item.Name = itemName;
//	d_tempTemplateData.push_back(item);
//	d_currentTempDataIter = d_tempTemplateData.end() - 1;
//}

void CT3TemplateWidget::closeEvent(QCloseEvent * event)
{
	if (d_templateData != d_tempTemplateData)
	{
		int ret = messageBoxOkCancelDiscard(QString::fromLocal8Bit("配置已经更改！"),
			QString::fromLocal8Bit("是否保存配置"));

		switch (ret)
		{
		case QMessageBox::Save:
			saveTemplateDataToFile();
			event->accept();
			break;
		case QMessageBox::Discard:
			event->accept();
			break;
		case QMessageBox::Cancel:
			event->ignore();
			return;
		default:
			break;
		}
	}
}

void CT3TemplateWidget::on_addButton_clicked()
{
	ct3AddModifyDialog* dlg = new ct3AddModifyDialog(d_tempTemplateData, d_tempTemplateData.end(), d_ct3Data, this);
	dlg->show();
}