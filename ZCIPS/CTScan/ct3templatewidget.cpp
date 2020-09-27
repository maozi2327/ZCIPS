#include "stdafx.h"
#include "ct3templatewidget.h"
#include "../Public/headers/SetupData.h"

CT3TemplateWidget::CT3TemplateWidget(const CT3Data& _ct3Data, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initiliseCt3Controls(_ct3Data);
	QString str;
	d_templateFileName = str.sprintf("%d_%d_ct3Template", _ct3Data.Ray, _ct3Data.Det);
	loadTemplateData();
}

CT3TemplateWidget::~CT3TemplateWidget()
{
}

const unsigned char EQUALLAYER = 1;
const unsigned char MULTILAYER = 0;
const unsigned int FILEHEAD = 0XAAAAAAA3;

struct HeadData
{
	unsigned int fileHead = FILEHEAD;
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

	if (!file.open(QIODevice::ReadOnly))
	{
		LOG_ERROR("������CT�����ļ���" + d_templateFileName);
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

	for (int item = 0; item != itemNumber; ++item)
	{
		HeadData data;
		file.read((char*)(&data), sizeof(HeadData));
		Ct3TemplateData itemData;
		itemData.Name = QString::fromLocal8Bit(data.Name);
		memcpy(&itemData.MutilayerOrEqualLayer,
			&data.MutilayerOrEqualLayer,
			(char*)(&data.LayerNumber) - (char*)(&data.MutilayerOrEqualLayer));

		for (int i = 0; i != data.LayerNumber; ++i)
		{	
			float pos;
			file.read((char*)(&pos), sizeof(float));
			itemData.LayerPos.push_back(pos);
		}

		d_templateData.push_back(itemData);
	}

	file.flush();
	file.close();
	return false;
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
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("����"));
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("���ȼ��"));
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("��㲻�ȼ��"));
	ui.ct3MultiLayerComboBox->setCurrentText(QString::fromLocal8Bit("����"));
}

void CT3TemplateWidget::saveTemplateDataToFile()
{
	size_t fileSize = sizeof(unsigned int);
	std::vector<std::pair<char*, size_t>> dataVec;

	for (auto& item : d_templateData)
	{
		auto layerNumber = item.LayerPos.size();
		auto dataSize = sizeof(HeadData) + layerNumber * sizeof(float);
		HeadData* data = (HeadData*)malloc(dataSize);
		data->LayerNumber = layerNumber;
		memcpy(&data->MutilayerOrEqualLayer,
			&item.MutilayerOrEqualLayer,
			(char*)(&data->LayerNumber) - (char*)(&data->MutilayerOrEqualLayer));
		std::copy(item.LayerPos.begin(), item.LayerPos.end(), data->LayerPos);
		fileSize += dataSize;
		dataVec.push_back({ (char*)data, dataSize });
	}

	unsigned int* fileMem = (unsigned int*)malloc(fileSize);
	*fileMem = FILEHEAD;
	char* fileDataHead = (char*)fileMem + sizeof(unsigned int);
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

	unsigned int itemNumber;
	file.write((char*)(fileMem), fileSize);
	free(fileMem);
}

void CT3TemplateWidget::on_saveButton_clicked()
{
	//�������б������Ŀ��Ȼ�󱣴浽���������У����������������ݱ��浽�ļ�
	QString itemName = ui.templateNameEdit->text();
	auto itr =  std::find_if(d_templateData.begin(), d_templateData.end(),
		[&itemName](Ct3TemplateData& item) { return item.Name == itemName; });

	if(itr == d_templateData.end())
	{
		LOG_INFO("�Ѵ���ͬ�������");
		return;
	}

	auto layerNumber = ui.ct3SpaceNumLineEdit->text().toInt();

	if (layerNumber > 500)
	{
		LOG_ERROR("�ֲ���������%d", layerNumber);
		return;
	}

	Ct3TemplateData templateData;

	if (ui.ct3MultiLayerComboBox->currentText() == QString::fromLocal8Bit("���"))
		templateData.MutilayerOrEqualLayer = MULTILAYER;
	else if (ui.ct3MultiLayerComboBox->currentText() == QString::fromLocal8Bit("�ȼ��"))
		templateData.MutilayerOrEqualLayer = EQUALLAYER;

	templateData.Name = itemName;
	templateData.Matrix = ui.ct3MatrixComboBox->currentText().toInt();
	templateData.View = ui.ct3ViewComboBox->currentText().toInt();
	templateData.SampleTime = ui.ct3SampleTimeComboBox->currentText().toInt();
	templateData.Orientation = ui.ct3AngleLineEdit->text().toFloat();

	if (templateData.MutilayerOrEqualLayer == MULTILAYER)
	{
		templateData.ecqualLayerNumber = ui.ct3SpaceNumLineEdit->text().toInt();
		templateData.layerSpace = ui.ct3LayerSpaceLineEdit->text().toFloat();
		templateData.LayerPos.push_back(ui.ct3LayerPosEdit->text().toFloat());
	}
	else if (templateData.MutilayerOrEqualLayer == EQUALLAYER)
	{
		auto layerNumber = ui.ct3LayerPosListWidget->count();

		for (int i = 0; i != layerNumber; ++i)
		{
			QListWidgetItem* it = ui.ct3LayerPosListWidget->item(i);
			templateData.LayerPos.push_back(it->text().toFloat());
		}
	}

	d_templateData.push_back(templateData);
	saveTemplateDataToFile();
}


//auto item = new QListWidgetItem();
//item->setText(QString::number(double(), 'g', 2));