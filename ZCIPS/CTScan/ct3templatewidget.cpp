#include "stdafx.h"
#include "ct3templatewidget.h"
#include "../Public/headers/SetupData.h"

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

CT3TemplateWidget::CT3TemplateWidget(const CT3Data& _ct3Data, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	initiliseCt3Controls(_ct3Data);
	QString str;
	d_templateFileName = str.sprintf("%d_%d_ct3Template", _ct3Data.Ray, _ct3Data.Det);
	d_saved = true;
	loadTemplateData();
	ui.ct3ItemNameListWidget->addItem(new QListWidgetItem(""));  //����һ���յ�item���ڱ༭
	connect(ui.ct3LayerPosEdit, &QLineEdit::returnPressed, this, &CT3TemplateWidget::on_ct3LayerPosEdit_returnd);
	ui.ct3ItemNameListWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
}

CT3TemplateWidget::~CT3TemplateWidget()
{
}

const unsigned char EQUALLAYER = 1;
const unsigned char MULTILAYER = 0;
const unsigned int FILEHEAD = 0XAAAAAAA3;

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

	if (itemNumber != 0)
		ui.ct3ItemNameListWidget->setCurrentRow(0);

	return true;
}

template<typename T>
void addItemToMatixVieSample(T& _data, QComboBox* _matrix, QComboBox* _view, QComboBox* _sampleTime,
	int _rayNum, int _detNum)
{
	for (auto& value : _data.Matrix)
		_matrix->addItem(QString::number(value));

	for (auto& value : _data.View)
		_view->addItem(QString::number(value));

	for (auto& value : _data.SampleTime)
		_sampleTime->addItem(QString::number(value));
}

void CT3TemplateWidget::initiliseCt3Controls(const CT3Data & _data)
{
	addItemToMatixVieSample(_data, ui.ct3MatrixComboBox, ui.ct3ViewComboBox,
		ui.ct3SampleTimeComboBox, d_rayNum, d_detNum);
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("���ȼ��"));
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("��㲻�ȼ��"));
	ui.ct3MultiLayerComboBox->setCurrentText(QString::fromLocal8Bit("��㲻�ȼ��"));
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
void CT3TemplateWidget::refreshPosListWidget()
{
	ui.ct3LayerPosListWidget->clear();

	for (auto pos : d_currentTempDataIter->LayerPos)
	{
		QListWidgetItem* item = new QListWidgetItem(QString::number(pos, 'f', 2));
		ui.ct3LayerPosListWidget->addItem(item);
	}
}
void CT3TemplateWidget::saveItem()
{
	//�������б������Ŀ��Ȼ�󱣴浽���������У����������������ݱ��浽�ļ�
	QString itemName = ui.templateNameEdit->text();

	if (itemName.size() == 0)
	{
		LOG_ERROR("��������������Ϊ�գ�" + d_templateFileName);
		return;
	}

	auto itr = std::find_if(d_tempTemplateData.begin(), d_tempTemplateData.end(),
		[&itemName](Ct3TemplateData& item) { return item.Name == itemName; });

	if (itr != d_tempTemplateData.end())
	{
		LOG_INFO("�Ѵ���ͬ�������");
		return;
	}

	Ct3TemplateData templateData;
	int layerNumber = 0;

	if (ui.ct3MultiLayerComboBox->currentText() == QString::fromLocal8Bit("���ȼ��"))
	{
		layerNumber = ui.ct3LayerNumLineEdit->text().toInt();
		templateData.MutilayerOrEqualLayer = EQUALLAYER;
	}
	else if (ui.ct3MultiLayerComboBox->currentText() == QString::fromLocal8Bit("��㲻�ȼ��"))
	{
		layerNumber = ui.ct3LayerPosListWidget->count();

		if (layerNumber == 0)
		{
			LOG_INFO("������ϲ�λ��");
			return;
		}

		templateData.MutilayerOrEqualLayer = MULTILAYER;
	}

	if (layerNumber > 500)
	{
		LOG_ERROR("�ֲ���������%d", layerNumber);
		return;
	}

	templateData.Name = itemName;
	templateData.Matrix = ui.ct3MatrixComboBox->currentText().toInt();
	templateData.View = ui.ct3ViewComboBox->currentText().toInt();
	templateData.SampleTime = ui.ct3SampleTimeComboBox->currentText().toInt();
	templateData.Orientation = ui.ct3AngleLineEdit->text().toFloat();

	if (templateData.MutilayerOrEqualLayer == MULTILAYER)
	{
		auto layerNumber = ui.ct3LayerPosListWidget->count();

		for (int i = 0; i != layerNumber; ++i)
		{
			QListWidgetItem* it = ui.ct3LayerPosListWidget->item(i);
			templateData.LayerPos.insert(it->text().toFloat());
		}

	}
	else if (templateData.MutilayerOrEqualLayer == EQUALLAYER)
	{
		templateData.ecqualLayerNumber = ui.ct3LayerNumLineEdit->text().toInt();
		templateData.layerSpace = ui.ct3LayerSpaceLineEdit->text().toFloat();
		templateData.LayerPos.insert(ui.ct3LayerPosEdit->text().toFloat());
	}


	d_tempTemplateData.push_back(templateData);
	d_currentTempDataIter = d_tempTemplateData.end() - 1;
	QListWidgetItem* item = new QListWidgetItem(itemName);
	ui.ct3ItemNameListWidget->addItem(item);
}
void CT3TemplateWidget::on_deleteButton_clicked()
{
	int ret = messageBoxOkCancel(QString::fromLocal8Bit("ȷ��ɾ����"), QString::fromLocal8Bit("ȷ��/ȡ��"));
	
	if (ret = QMessageBox::Cancel)
		return;

	int row = ui.ct3ItemNameListWidget->currentRow();
	QListWidgetItem* item = ui.ct3ItemNameListWidget->takeItem(row);
	delete item;
	d_tempTemplateData.erase(d_tempTemplateData.begin() + row);
	saveTemplateDataToFile();
}
void CT3TemplateWidget::on_saveButton_clicked()
{
	if (!d_saved)
	{
		saveItem();
		saveTemplateDataToFile();
	}
}

void CT3TemplateWidget::on_ct3MultiLayerComboBox_currentIndexChanged(const QString& _text)
{
	if (_text == QString::fromLocal8Bit("���ȼ��"))
	{
		ui.ct3LayerSpaceLineEdit->show();
		ui.ct3LayerNumLineEdit->show();
		ui.ct3SpaceNumLabel->show();
		ui.ct3LayerSpaceLabel->show();
		ui.label_8->setText(QString::fromLocal8Bit("�ײ�λ��"));
		ui.ct3LayerPosListWidget->setEnabled(false);
		ui.ct3LayerPosListWidget->clear();
	}
	else if (_text == QString::fromLocal8Bit("��㲻�ȼ��"))
	{
		ui.ct3LayerSpaceLineEdit->hide();
		ui.ct3LayerNumLineEdit->hide();
		ui.ct3SpaceNumLabel->hide();
		ui.ct3LayerSpaceLabel->hide();
		ui.label_8->setText(QString::fromLocal8Bit("�ϲ�λ��"));
		ui.ct3LayerPosListWidget->setEnabled(true);
	}

	d_saved = false;
}

void CT3TemplateWidget::on_ct3LayerPosEdit_returnd()
{
	QString valueText = ui.ct3LayerPosEdit->text();
	bool succeed = false;
	auto value = valueText.toFloat(&succeed);

	if (!succeed)
		return;

	if (d_currentTempDataIter->LayerPos.find(value) == d_currentTempDataIter->LayerPos.end())
	{
		d_currentTempDataIter->LayerPos.insert(value);
		refreshPosListWidget();
	}
	else
		LOG_ERROR("�Ѿ�������ͬ����λ��");

	ui.ct3LayerPosEdit->selectAll();
	d_saved = false;
}

void CT3TemplateWidget::on_ct3LayerPosListWidget_itemDoubleClicked(QListWidgetItem * _item)
{
	float pos = _item->text().toFloat();
	auto it = std::find_if(d_currentTempDataIter->LayerPos.begin(), d_currentTempDataIter->LayerPos.end(),
		[pos](float value) { return std::abs(value - pos) < 0.01; });
	d_currentTempDataIter->LayerPos.erase(it);
	refreshPosListWidget();
	d_saved = false;
}


void CT3TemplateWidget::on_ct3ItemNameListWidget_currentRowChanged(int _currentRow)
{
	if (!d_saved)
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromLocal8Bit("�����Ѿ����ģ�"));
		msgBox.setInformativeText(QString::fromLocal8Bit("�Ƿ񱣴�����"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();

		switch (ret)
		{
		case QMessageBox::Save:
			saveItem();
			saveTemplateDataToFile();
			break;
		case QMessageBox::Discard:
			d_tempTemplateData = d_templateData;
			break;
		default:
			break;
		}
	}

	d_lastNameListRow = _currentRow;
	Ct3TemplateData dataItem = d_tempTemplateData[_currentRow];

	if (dataItem.MutilayerOrEqualLayer == MULTILAYER)
	{
		ui.ct3MultiLayerComboBox->setCurrentText(QString::fromLocal8Bit("��㲻�ȼ��"));
		ui.ct3LayerPosListWidget->clear();

		for (auto pos : dataItem.LayerPos)
		{
			QListWidgetItem* item = new QListWidgetItem(QString::number(pos, 'f', 2));
			ui.ct3LayerPosListWidget->addItem(item);
		}
	}
	else if (dataItem.MutilayerOrEqualLayer == EQUALLAYER)
	{
		ui.ct3MultiLayerComboBox->setCurrentText(QString::fromLocal8Bit("���ȼ��"));
		ui.ct3LayerSpaceLineEdit->setText(QString::number(dataItem.layerSpace, 'f', 2));
		ui.ct3LayerNumLineEdit->setText(QString::number(dataItem.ecqualLayerNumber));
		ui.ct3LayerPosEdit->setText(QString::number(*dataItem.LayerPos.begin()));
	}

	ui.ct3MatrixComboBox->setCurrentText(QString::number(dataItem.Matrix));
	ui.ct3ViewComboBox->setCurrentText(QString::number(dataItem.View));
	ui.ct3SampleTimeComboBox->setCurrentText(QString::number(dataItem.SampleTime));
	ui.ct3AngleLineEdit->setText(QString::number(dataItem.Orientation, 'f', 2));
}

void CT3TemplateWidget::on_ct3ItemNameListWidget_itemDoubleClicked(QListWidgetItem * _item)
{
	
}

void CT3TemplateWidget::on_ct3MatrixComboBox_currentIndexChanged(const QString & _text)
{
	d_saved = false;
}

void CT3TemplateWidget::on_ct3ViewComboBox_currentIndexChanged(const QString & _text)
{
	d_saved = false;
}

void CT3TemplateWidget::on_ct3SampleTimeComboBox_currentIndexChanged(const QString & _text)
{
	d_saved = false;
}
bool CT3TemplateWidget::eventFilter(QObject* _object, QEvent* _event)
{
	if (_object == ui.ct3AngleLineEdit)
	{
		if (_event->type() == QEvent::FocusIn)
			d_angleEditText = ui.ct3AngleLineEdit->text();
		else if (_event->type() == QEvent::FocusOut)
		{
			auto text = ui.ct3AngleLineEdit->text();
			bool succeed = false;
			auto value = text.toFloat(&succeed);

			if (!succeed)
			{
				int ret = messageBox(QString::fromLocal8Bit("�������ʹ���"), QString::fromLocal8Bit("����������"));
				ui.ct3AngleLineEdit->setText(d_angleEditText);
				ui.ct3AngleLineEdit->selectAll();
				return true;
			}

			d_currentTempDataIter->Orientation = value;
			d_saved = false;
		}
	}
	else if (_object == ui.ct3LayerNumLineEdit)
	{
		if (_event->type() == QEvent::FocusIn)
			d_layerNumberEditText = ui.ct3LayerNumLineEdit->text();
		else if (_event->type() == QEvent::FocusOut)
		{
			auto text = ui.ct3LayerNumLineEdit->text();
			bool succeed = false;
			auto value = text.toInt(&succeed);

			if (!succeed)
			{
				int ret = messageBox(QString::fromLocal8Bit("�������ʹ���"), QString::fromLocal8Bit("����������"));
				ui.ct3LayerNumLineEdit->setText(d_layerNumberEditText);
				ui.ct3LayerNumLineEdit->selectAll();
				return true;
			}

			d_currentTempDataIter->ecqualLayerNumber = value;
			d_saved = false;
		}
	}

	return true;
}