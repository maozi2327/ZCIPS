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
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Discard);
	return msgBox.exec();
}

CT3TemplateWidget::CT3TemplateWidget(const CT3Data& _ct3Data, QWidget *parent)
	: QDialog(parent), d_ct3Data(_ct3Data)
{
	ui.setupUi(this);
	//ui.ct3MultiLayerLabel->setEnabled(false);
	//ui.ct3LayerPosListWidget->setEnabled(false);
	//ui.ct3LayerSpaceLabel->setEnabled(false);
	//ui.ct3LayerNumLabel->setEnabled(false);
	//ui.ct3LayerPosEdit->setEnabled(false);
	//ui.ct3MatrixLabel->setEnabled(false);
	//ui.ct3ViewLabel->setEnabled(false);
	//ui.ct3SampleTimeLabel->setEnabled(false);
	//ui.ct3AngleLabel->setEnabled(false);
	QString str;
	d_templateFileName = str.sprintf("%d_%d_ct3Template", _ct3Data.Ray, _ct3Data.Det);
	loadTemplateData(); 
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
		LOG_INFO("�����������ļ���" + d_templateFileName);

	if (!file.open(QIODevice::ReadOnly))
	{
		LOG_ERROR("������CT�����ļ�ʧ�ܣ�" + d_templateFileName);
		return false;
	}

	unsigned int fileHead = 0;
	file.read((char*)(&fileHead), sizeof(unsigned int));

	if(fileHead != FILEHEAD)
	{
		LOG_ERROR("����CT�����ļ���ʽ����" + d_templateFileName);
		return false;
	}

	unsigned int itemNumber;
	file.read((char*)(&itemNumber), sizeof(unsigned int));
	
	if(itemNumber > 500)
	{
		LOG_ERROR("����CT�����ļ���������������" + d_templateFileName);
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
		LOG_INFO("�����������ļ���" + d_templateFileName + "���Ѿ��Զ����������ļ�");

	QFile file;
	file.setFileName(d_templateFileName);

	if (!file.open(QIODevice::WriteOnly))
	{
		LOG_ERROR("������CT����ʧ�ܣ�" + d_templateFileName);
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
	//if (d_currentTempDataIter->MutilayerOrEqualLayer == MULTILAYER)
	//{
	//	if (d_currentTempDataIter->LayerPos.size() == 0)
	//	{
	//		messageBox(QString::fromLocal8Bit("����������"),
	//			QString::fromLocal8Bit("��������ȷ����"));
	//		ui.ct3ItemNameListWidget->setCurrentRow(d_lastNameListRow);
	//		return;
	//	}
	//}
	//else if (d_currentTempDataIter->MutilayerOrEqualLayer == EQUALLAYER)
	//{
	//	if (d_currentTempDataIter->LayerPos.size() == 0 |
	//		d_currentTempDataIter->ecqualLayerNumber == 0 |
	//		d_currentTempDataIter->layerSpace == 0)
	//	{
	//		messageBox(QString::fromLocal8Bit("����������"),
	//			QString::fromLocal8Bit("��������ȷ����"));
	//		ui.ct3ItemNameListWidget->setCurrentRow(d_lastNameListRow);
	//		return;
	//	}
	//}
	saveTemplateDataToFile();
}

void CT3TemplateWidget::on_ct3ItemNameListWidget_itemDoubleClicked(QListWidgetItem * _item)
{
	auto row = ui.ct3ItemNameListWidget->row(_item);
	ct3AddModifyDialog* dlg = new ct3AddModifyDialog(d_tempTemplateData, d_tempTemplateData.begin() + row, d_ct3Data, this);
	
	if (dlg->exec() == QDialog::Accepted)
	{
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
		ui.ct3MultiLayerLabel->setText(QString::fromLocal8Bit("��㲻�ȼ��"));
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
		ui.label_8->setText(QString::fromLocal8Bit("�ϲ�λ��"));
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
		ui.label_8->setText(QString::fromLocal8Bit("�ײ�λ��"));
		ui.ct3MultiLayerLabel->setText(QString::fromLocal8Bit("���ȼ��"));
		ui.ct3LayerSpaceValueLabel->setText(QString::number(dataItem.layerSpace, 'f', 2));
		ui.ct3LayerNumValueLabel->setText(QString::number(dataItem.ecqualLayerNumber));
		ui.ct3LayerPosValueLabel->setText(QString::number(*dataItem.LayerPos.begin()));
	}

	ui.ct3MatrixLabel->setText(QString::number(dataItem.Matrix));
	ui.ct3ViewLabel->setText(QString::number(dataItem.View));
	ui.ct3SampleTimeLabel->setText(QString::number(dataItem.SampleTime));
	ui.ct3AngleLabel->setText(QString::number(dataItem.Orientation, 'f', 2));
}
//void CT3TemplateWidget::on_ct3ItemNameListWidget_itemChanged(QListWidgetItem * _item)
//{
//	//�������б������Ŀ��Ȼ�󱣴浽���������У����������������ݱ��浽�ļ�
//	QString itemName = ui.templateNameEdit->text();
////	if (itemName.size() == 0)
//	{
//		LOG_ERROR("��������������Ϊ�գ�" + d_templateFileName);
//		return;
//	}
////	auto itr = std::find_if(d_tempTemplateData.begin(), d_tempTemplateData.end(),
//		[&itemName](Ct3TemplateData& item) { return item.Name == itemName; });
//
//	if (itr != d_tempTemplateData.end())
//	{
//		LOG_INFO("�Ѵ���ͬ�������");
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
		int ret = messageBoxOkCancelDiscard(QString::fromLocal8Bit("�����Ѿ����ģ�"),
			QString::fromLocal8Bit("�Ƿ񱣴�����"));

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
	
	if(dlg->exec() == QDialog::Accepted);
		refresh(d_tempTemplateData.size() - 1);
}