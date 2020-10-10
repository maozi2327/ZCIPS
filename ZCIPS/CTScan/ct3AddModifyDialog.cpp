#include "stdafx.h"
#include "ct3AddModifyDialog.h"
#include "../Public/headers/SetupData.h"
#include "ct3templatewidget.h"

ct3AddModifyDialog::ct3AddModifyDialog(std::vector<Ct3TemplateData>& _tempData, 
	std::vector<Ct3TemplateData>::iterator _itr, const CT3Data& _ct3Data, QWidget *parent)
	: QDialog(parent), d_itr(_itr), d_tempData(_tempData), d_tempItem(new Ct3TemplateData)
	, d_modified(false), d_widgetInitilised(false)
{
	ui.setupUi(this); 	
	initiliseCt3Controls(_ct3Data);

	if (d_itr != _tempData.end())
	{
		*d_tempItem = *d_itr;

		if (d_tempItem->MutilayerOrEqualLayer == MULTILAYER)
		{
			ui.ct3MultiLayerComboBox->setCurrentText(QString::fromLocal8Bit("��㲻�ȼ��"));
			ui.ct3LayerPosListWidget->show();
			ui.ct3LayerPosListWidget->clear();

			for (auto pos : d_tempItem->LayerPos)
			{
				QListWidgetItem* item = new QListWidgetItem(QString::number(pos, 'f', 2));
				ui.ct3LayerPosListWidget->addItem(item);
			}
		}
		else if (d_tempItem->MutilayerOrEqualLayer == EQUALLAYER)
		{
			ui.ct3MultiLayerComboBox->setCurrentText(QString::fromLocal8Bit("���ȼ��"));
			ui.ct3LayerSpaceLineEdit->setText(QString::number(d_tempItem->layerSpace, 'f', 2));
			ui.ct3LayerNumLineEdit->setText(QString::number(d_tempItem->ecqualLayerNumber));
			ui.ct3LayerPosEdit->setText(QString::number(*d_tempItem->LayerPos.begin()));
		}

		ui.templateNameEdit->setText(d_tempItem->Name);
		ui.ct3MatrixComboBox->setCurrentText(QString::number(d_tempItem->Matrix));
		ui.ct3ViewComboBox->setCurrentText(QString::number(d_tempItem->View));
		ui.ct3SampleTimeComboBox->setCurrentText(QString::number(d_tempItem->SampleTime));
		ui.ct3AngleLineEdit->setText(QString::number(d_tempItem->Orientation, 'f', 2));
		d_layerPos = d_itr->LayerPos;
	}

	d_widgetInitilised = true;
}

ct3AddModifyDialog::~ct3AddModifyDialog()
{

}

template<typename T>
void addItemToMatixVieSample(T& _data, QComboBox* _matrix, QComboBox* _view, QComboBox* _sampleTime)
{
	for (auto& value : _data.Matrix)
		_matrix->addItem(QString::number(value));

	for (auto& value : _data.View)
		_view->addItem(QString::number(value));

	for (auto& value : _data.SampleTime)
		_sampleTime->addItem(QString::number(value));
}

void ct3AddModifyDialog::initiliseCt3Controls(const CT3Data & _data)
{
	addItemToMatixVieSample(_data, ui.ct3MatrixComboBox, ui.ct3ViewComboBox,
		ui.ct3SampleTimeComboBox);
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("���ȼ��"));
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("��㲻�ȼ��"));
}

void ct3AddModifyDialog::on_ct3MatrixComboBox_currentIndexChanged(const QString & _text)
{
	if(d_widgetInitilised)
		d_modified = true;
}

void ct3AddModifyDialog::on_ct3ViewComboBox_currentIndexChanged(const QString & _text)
{
	if (d_widgetInitilised)
		d_modified = true;
}

void ct3AddModifyDialog::on_ct3SampleTimeComboBox_currentIndexChanged(const QString & _text)
{
	if (d_widgetInitilised)
		d_modified = true;
}

void ct3AddModifyDialog::on_okButton_clicked()
{
	if (save())
		accept();
	else
		return;
}

void ct3AddModifyDialog::on_cancelButton_clicked()
{
	reject();
}

void ct3AddModifyDialog::refreshPosListWidget()
{
	ui.ct3LayerPosListWidget->clear();

	for (auto pos : d_layerPos)
	{
		QListWidgetItem* item = new QListWidgetItem(QString::number(pos, 'f', 2));
		ui.ct3LayerPosListWidget->addItem(item);
	}
}

bool ct3AddModifyDialog::save()
{
	QString itemName = ui.templateNameEdit->text();

	if (itemName.size() == 0)
	{
		messageBox(QString::fromLocal8Bit("����������Ϊ��"),
			QString::fromLocal8Bit("������������"));
		return false;
	}

	auto itr = std::find_if(d_tempData.begin(), d_itr,
		[&itemName](Ct3TemplateData& item) { return item.Name == itemName; });

	if (itr == d_tempData.end() || itr == d_itr)
	{
		if (d_itr != d_tempData.end())
		{
			auto itr = std::find_if(d_itr + 1, d_tempData.end(),
				[&itemName](Ct3TemplateData& item) { return item.Name == itemName; });

			if (itr != d_tempData.end())
			{
				messageBox(QString::fromLocal8Bit("�Ѵ���ͬ����"),
					QString::fromLocal8Bit("��������ȷ��������"));
				return false;
			}
		}
	}
	else
	{
		messageBox(QString::fromLocal8Bit("�Ѵ���ͬ����"),
			QString::fromLocal8Bit("��������ȷ��������"));
		return false;
	}

	if (ui.ct3MultiLayerComboBox->currentText() == QString::fromLocal8Bit("��㲻�ȼ��"))
	{
		if (ui.ct3LayerPosListWidget->count() == 0)
		{
			messageBox(QString::fromLocal8Bit("����������"),
				QString::fromLocal8Bit("������ϲ�����"));
			return false;
		}

		d_tempItem->MutilayerOrEqualLayer = MULTILAYER;
		d_tempItem->LayerPos = d_layerPos;
	}
	else if (ui.ct3MultiLayerComboBox->currentText() == QString::fromLocal8Bit("���ȼ��"))
	{
		if (ui.ct3LayerPosEdit->text().size() == 0 |
			ui.ct3LayerNumLineEdit->text().size() == 0 |
			ui.ct3LayerSpaceLineEdit->text().size() == 0)
		{
			messageBox(QString::fromLocal8Bit("����������"),
				QString::fromLocal8Bit("������ȱ�ٵĲ���"));
			return false;
		}

		if (ui.ct3LayerNumLineEdit->text().toInt() > 500)
		{
			int ret = messageBox(QString::fromLocal8Bit("����ֲ��������ޣ�"), QString::fromLocal8Bit("���������룡"));
			ui.ct3LayerNumLineEdit->selectAll();
			return false;
		}
		else if (ui.ct3LayerSpaceLineEdit->text().toFloat() < 0.01)
		{
			int ret = messageBox(QString::fromLocal8Bit("����ϲ����С����"), QString::fromLocal8Bit("���������룡"));
			ui.ct3LayerSpaceLineEdit->selectAll();
			return false;
		}

		d_tempItem->MutilayerOrEqualLayer = EQUALLAYER;
		d_tempItem->LayerPos.insert(ui.ct3LayerPosEdit->text().toFloat());
		d_tempItem->ecqualLayerNumber = ui.ct3LayerNumLineEdit->text().toFloat();
		d_tempItem->layerSpace = ui.ct3LayerSpaceLineEdit->text().toFloat();
	}

	d_tempItem->Name = itemName;
	d_tempItem->Orientation = ui.ct3AngleLineEdit->text().toFloat();
	d_tempItem->Matrix = ui.ct3MatrixComboBox->currentText().toInt();
	d_tempItem->View = ui.ct3ViewComboBox->currentText().toInt();
	d_tempItem->SampleTime = ui.ct3SampleTimeComboBox->currentText().toInt();

	if (d_itr == d_tempData.end())
		d_tempData.push_back(*d_tempItem);
	else
		*d_itr = *d_tempItem;

	return true;
}

void ct3AddModifyDialog::on_ct3LayerPosEdit_returnPressed()
{
	QString valueText = ui.ct3LayerPosEdit->text();
	bool succeed = false;
	auto pos = valueText.toFloat(&succeed);

	if (!succeed)
	{
		ui.ct3LayerPosEdit->selectAll();
		return;
	}
	
	ui.ct3LayerPosEdit->selectAll();
	auto it = std::find_if(d_layerPos.begin(), d_layerPos.end(),
		[pos](float value) { return std::abs(value - pos) < 0.01; });

	if (it == d_layerPos.end())
	{
		d_modified = true;
		d_layerPos.insert(pos);
		refreshPosListWidget();
	}
	else
	{
		messageBox(QString::fromLocal8Bit("�ϲ����겻����ͬ"),
			QString::fromLocal8Bit("�����벻ͬ�ϲ�����"));
		return;
	}
}

void ct3AddModifyDialog::on_ct3LayerPosListWidget_itemDoubleClicked(QListWidgetItem * _item)
{
	float pos = _item->text().toFloat();
	auto it = std::find_if(d_layerPos.begin(), d_layerPos.end(),
		[pos](float value) { return std::abs(value - pos) < 0.01; });
	d_layerPos.erase(it);
	refreshPosListWidget();
	d_modified = true;
}

void ct3AddModifyDialog::on_ct3MultiLayerComboBox_currentIndexChanged(const QString& _text)
{
	if (_text == QString::fromLocal8Bit("���ȼ��"))
	{
		ui.ct3LayerSpaceLineEdit->show();
		ui.ct3LayerNumLineEdit->show();
		ui.ct3SpaceNumLabel->show();
		ui.ct3LayerSpaceLabel->show();
		ui.ct3LayerPosListWidget->hide();
		ui.label_8->setText(QString::fromLocal8Bit("�ײ�λ��"));
		refreshPosListWidget();
	}
	else if (_text == QString::fromLocal8Bit("��㲻�ȼ��"))
	{
		ui.ct3LayerSpaceLineEdit->hide();
		ui.ct3LayerNumLineEdit->hide();
		ui.ct3SpaceNumLabel->hide();
		ui.ct3LayerSpaceLabel->hide();
		ui.ct3LayerPosListWidget->show();
		ui.label_8->setText(QString::fromLocal8Bit("�ϲ�λ��"));
	}

	if(d_widgetInitilised)
		d_modified = true;
}

void ct3AddModifyDialog::on_templateNameEdit_textEdited(const QString& _text)
{
	d_modified = true;
}
void ct3AddModifyDialog::on_ct3AngleLineEdit_textEdited(const QString& _text)
{
	bool succeed = false;
	auto pos = _text.toFloat(&succeed);

	if (!succeed)
	{
		ui.ct3AngleLineEdit->clear();
		messageBox(QString::fromLocal8Bit("��ʽ����"), QString::fromLocal8Bit("��������ȷ������"));
		return;
	}

	d_modified = true;
}

bool isInterger(const QString& _text)
{
	const char rawChar = _text.back().toLatin1();

	if (rawChar > '9' || rawChar < '0')
		return false;

	return true;
}

void ct3AddModifyDialog::on_ct3LayerSpaceLineEdit_textEdited(const QString& _text)
{
	bool succeed = false;
	auto pos = _text.toFloat(&succeed);

	if (!succeed)
	{
		ui.ct3LayerSpaceLineEdit->clear();
		messageBox(QString::fromLocal8Bit("��ʽ����"), QString::fromLocal8Bit("��������ȷ������"));
		return;
	}

	d_modified = true;
}
void ct3AddModifyDialog::on_ct3LayerNumLineEdit_textEdited(const QString& _text)
{
	if (!isInterger(_text))
	{
		ui.ct3LayerNumLineEdit->clear();
		messageBox(QString::fromLocal8Bit("��ʽ����"), QString::fromLocal8Bit("��������ȷ������"));
		return;
	}

	d_modified = true;
}

void ct3AddModifyDialog::closeEvent(QCloseEvent * event)
{
	if (d_modified)
	{
		int ret = messageBoxOkCancelDiscard(QString::fromLocal8Bit("�����Ѿ��仯"),
			QString::fromLocal8Bit("�Ƿ񱣴棿"));

		switch (ret)
		{
		case QMessageBox::Save:

			if (save())
			{
				event->accept();
				accept();
			}
			else
				event->ignore();;

			break;
		case QMessageBox::Discard:
			event->accept();	
			reject();
			break;
		case QMessageBox::Cancel:
			event->ignore();
			return;
		default:
			break;
		}
	}
	else
		event->accept();
}
