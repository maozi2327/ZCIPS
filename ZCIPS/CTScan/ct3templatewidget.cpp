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

	if (flag1)
	{
		bool flag2;

		if (t1.MutilayerOrEqualLayer == EQUALLAYER)
			flag2 =
			t1.ecqualLayerNumber == t2.ecqualLayerNumber &&
			t1.layerSpace == t2.layerSpace &&
			t1.LayerPos == t2.LayerPos;
		else if (t1.MutilayerOrEqualLayer == MULTILAYER)
			flag2 = t1.LayerPos == t2.LayerPos;

		return flag2;
	}

	return false;
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
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard);
	return msgBox.exec();
}

CT3TemplateWidget::CT3TemplateWidget(const CT3Data& _ct3Data, QWidget *parent)
	: QDialog(parent), d_ct3Data(_ct3Data), d_modified(false)
{
	ui.setupUi(this);
	QString str;
	d_templateFileName = str.sprintf("%d_%d_ct3Template", _ct3Data.Ray, _ct3Data.Det);
	loadTemplateData(); 
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

	if (itemNumber != 0) 
	{
		ui.ct3ItemNameListWidget->setCurrentRow(0);
		d_currentTempDataIter = d_tempTemplateData.begin();
	}
		
	return true;
}

bool CT3TemplateWidget::saveTemplateDataToFile()
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
		free(fileMem);
		LOG_ERROR("打开三代CT配置文件失败：" + d_templateFileName);
		messageBox(QString::fromLocal8Bit("打开三代CT配置文件失败：！"), 
			QString::fromLocal8Bit("保存失败！"));
		return false;
	}

	file.write((char*)(fileMem), fileSize);
	free(fileMem);
	file.flush();
	file.close();
	d_templateData = d_tempTemplateData;
	d_modified = false;
	return true;
}

void CT3TemplateWidget::on_deleteButton_clicked()
{
	int row = ui.ct3ItemNameListWidget->currentRow();
	QListWidgetItem* item = ui.ct3ItemNameListWidget->takeItem(row);
	delete item;
	d_tempTemplateData.erase(d_tempTemplateData.begin() + row);
}

void CT3TemplateWidget::refresh(int _row)
{
	ui.ct3ItemNameListWidget->clear();
	
	for (auto& item : d_tempTemplateData)
		ui.ct3ItemNameListWidget->addItem(item.Name);

	if (d_tempTemplateData.size() != 0) {
		on_ct3ItemNameListWidget_currentRowChanged(_row);
		d_currentTempDataIter = d_tempTemplateData.begin() + _row;
	}
}

void CT3TemplateWidget::on_saveButton_clicked()
{
	saveTemplateDataToFile();
	close();
}

void CT3TemplateWidget::on_ct3ItemNameListWidget_itemDoubleClicked(QListWidgetItem * _item)
{
	auto row = ui.ct3ItemNameListWidget->row(_item);
	ct3AddModifyDialog* dlg = new ct3AddModifyDialog(d_tempTemplateData, d_tempTemplateData.begin() + row, d_ct3Data, this);
	auto ret = dlg->exec();

	if (ret == QDialog::Accepted)
	{
		d_modified = true;
		_item->setText(d_tempTemplateData[row].Name);
		on_ct3ItemNameListWidget_currentRowChanged(row);
	}
}

void CT3TemplateWidget::on_ct3ItemNameListWidget_currentRowChanged(int _currentRow)
{
	if (d_tempTemplateData.size() == 0 || _currentRow < 0)
		return;
	
	d_lastNameListRow = _currentRow;
	Ct3TemplateData dataItem = d_tempTemplateData[_currentRow];

	if (dataItem.MutilayerOrEqualLayer == MULTILAYER)
	{
		ui.ct3MultiLayerLabel->setText(QString::fromLocal8Bit("多层不等间距"));
		ui.ct3LayerPosListWidget->show();
		ui.ct3LayerPosListWidget->clear();

		for (auto pos : dataItem.LayerPos)
		{
			QListWidgetItem* item = new QListWidgetItem(QString::number(pos, 'f', 2));
			ui.ct3LayerPosListWidget->addItem(item);
		}

		ui.ct3LayerSpaceLabel->hide();
		ui.ct3LayerNumValueLabel->hide();
		ui.ct3LayerPosValueLabel->hide();
		ui.ct3LayerSpaceLabel->hide();
		ui.ct3SpaceNumLabel->hide();
		ui.ct3LayerSpaceLabel->hide();
		ui.ct3LayerSpaceValueLabel->hide();
		ui.ct3LayerSpaceLabel->hide();
		ui.label_8->setText(QString::fromLocal8Bit("断层位置"));
	}
	else if (dataItem.MutilayerOrEqualLayer == EQUALLAYER)
	{
		ui.ct3LayerSpaceLabel->show();
		ui.ct3LayerNumValueLabel->show();
		ui.ct3LayerPosValueLabel->show();
		ui.ct3LayerSpaceLabel->show();
		ui.ct3SpaceNumLabel->show();
		ui.ct3LayerSpaceLabel->show();
		ui.ct3LayerSpaceValueLabel->show();
		ui.ct3LayerPosListWidget->hide();
		ui.label_8->setText(QString::fromLocal8Bit("首层位置"));
		ui.ct3MultiLayerLabel->setText(QString::fromLocal8Bit("多层等间距"));
		ui.ct3LayerSpaceValueLabel->setText(QString::number(dataItem.layerSpace, 'f', 2));
		ui.ct3LayerNumValueLabel->setText(QString::number(dataItem.ecqualLayerNumber));
		ui.ct3LayerPosValueLabel->setText(QString::number(*dataItem.LayerPos.begin()));
	}

	ui.ct3MatrixLabel->setText(QString::number(dataItem.Matrix));
	ui.ct3ViewLabel->setText(QString::number(dataItem.View));
	ui.ct3SampleTimeLabel->setText(QString::number(dataItem.SampleTime));
	ui.ct3AngleLabel->setText(QString::number(dataItem.Orientation, 'f', 2));
}

void CT3TemplateWidget::closeEvent(QCloseEvent * event)
{
	if (d_modified)
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
	
	if (dlg->exec() == QDialog::Accepted) 
	{
		d_modified = true;
		refresh(d_tempTemplateData.size() - 1);
	}
}