#include "stdafx.h"
#include "ct3AddModifyDialog.h"
#include "../Public/headers/SetupData.h"
#include "ct3templatewidget.h"

ct3AddModifyDialog::ct3AddModifyDialog(std::vector<Ct3TemplateData>& _tempData, 
	std::vector<Ct3TemplateData>::iterator _itr, const CT3Data& _ct3Data, QWidget *parent)
	: QDialog(parent), d_itr(_itr), d_tempData(_tempData), d_tempItem(new Ct3TemplateData)
{
	ui.setupUi(this); 	
	initiliseCt3Controls(_ct3Data);	

	if (d_itr != _tempData.end())
	{
		*d_tempItem = *d_itr;

		if (d_tempItem->MutilayerOrEqualLayer == MULTILAYER)
		{
			ui.ct3MultiLayerComboBox->setCurrentText(QString::fromLocal8Bit("多层不等间距"));
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
			ui.ct3MultiLayerComboBox->setCurrentText(QString::fromLocal8Bit("多层等间距"));
			ui.ct3LayerSpaceLineEdit->setText(QString::number(d_tempItem->layerSpace, 'f', 2));
			ui.ct3LayerNumLineEdit->setText(QString::number(d_tempItem->ecqualLayerNumber));
			ui.ct3LayerPosEdit->setText(QString::number(*d_tempItem->LayerPos.begin()));
		}

		ui.templateNameEdit->setText(d_tempItem->Name);
		ui.ct3MatrixComboBox->setCurrentText(QString::number(d_tempItem->Matrix));
		ui.ct3ViewComboBox->setCurrentText(QString::number(d_tempItem->View));
		ui.ct3SampleTimeComboBox->setCurrentText(QString::number(d_tempItem->SampleTime));
		ui.ct3AngleLineEdit->setText(QString::number(d_tempItem->Orientation, 'f', 2));
	}
		
	//ui.ct3LayerNumLineEdit->installEventFilter(this);
	//ui.ct3LayerSpaceLineEdit->installEventFilter(this);
	//ui.ct3AngleLineEdit->installEventFilter(this);
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
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("多层等间距"));
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("多层不等间距"));
}

void ct3AddModifyDialog::on_ct3MatrixComboBox_currentIndexChanged(const QString & _text)
{
	d_tempItem->Matrix = ui.ct3MatrixComboBox->currentText().toInt();
}

void ct3AddModifyDialog::on_ct3ViewComboBox_currentIndexChanged(const QString & _text)
{
	d_tempItem->View = ui.ct3ViewComboBox->currentText().toInt();
}

void ct3AddModifyDialog::on_ct3SampleTimeComboBox_currentIndexChanged(const QString & _text)
{
	d_tempItem->SampleTime = ui.ct3SampleTimeComboBox->currentText().toInt();
}

bool ct3AddModifyDialog::eventFilter(QObject* _object, QEvent* _event)
{
	if (_object == ui.ct3AngleLineEdit | _object == ui.ct3LayerSpaceLineEdit | _object == ui.ct3LayerNumLineEdit)
	{
		auto edit = static_cast<QLineEdit*>(_object);

		if (_event->type() == QEvent::FocusIn)
			d_angleEditText = edit->text();
		else if (_event->type() == QEvent::FocusOut)
		{
			auto text = edit->text();
			bool succeed = false;
			auto value = text.toFloat(&succeed);

			if (!succeed)
			{
				int ret = messageBox(QString::fromLocal8Bit("输入类型错误！"), QString::fromLocal8Bit("请重新输入"));
				edit->setText(d_angleEditText);
				edit->selectAll();
				return true;
			}

			if (_object == ui.ct3AngleLineEdit)
				;// d_currentTempDataIter->Orientation = value;
			else if (_object == ui.ct3LayerNumLineEdit)
			{
				if (value > 500)
				{
					int ret = messageBox(QString::fromLocal8Bit("输入分层数量超限！"), QString::fromLocal8Bit("请重新输入！"));
					ui.ct3LayerNumLineEdit->selectAll();
					return true;
				}
			}
			else if (_object == ui.ct3LayerSpaceLineEdit)
			{
				if (value < 0.000001)
				{
					int ret = messageBox(QString::fromLocal8Bit("输入断层间距大小错误！"), QString::fromLocal8Bit("请重新输入！"));
					ui.ct3LayerSpaceLineEdit->selectAll();
					return true;
				}
			}
		}

		return true;
	}
	//else if (_object = ui.ct3ItemNameListWidget)
	//{
	//	if (_event->type() == QEvent::KeyPress)
	//	{
	//		QKeyEvent* e1 = static_cast<QKeyEvent*>(_event);
	//		if (e1->key() == Qt::Key_Return)
	//		{
	//			auto fl = ui.ct3ItemNameListWidget->currentItem()->flags();
	//			if ( fl & Qt::ItemIsEditable);
	//				return true;
	//		}
	//	}
	//	return QWidget::eventFilter(_object, _event);
	//}
	else
		return QWidget::eventFilter(_object, _event);
}

void ct3AddModifyDialog::on_okButton_clicked()
{
	QString itemName = ui.templateNameEdit->text();

	if (itemName.size() == 0)
	{
		messageBox(QString::fromLocal8Bit("配置名不能为空"),
			QString::fromLocal8Bit("请输入配置名"));
		return;
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
				messageBox(QString::fromLocal8Bit("已存在同名项"),
					QString::fromLocal8Bit("请输入正确的配置名"));
				return;
			}
		}
	}
	else
	{
		messageBox(QString::fromLocal8Bit("已存在同名项"),
			QString::fromLocal8Bit("请输入正确的配置名"));
		return;
	}

	if (ui.ct3MultiLayerComboBox->currentText() == QString::fromLocal8Bit("多层不等间距"))
	{
		if (ui.ct3LayerPosListWidget->count() == 0)
		{
			messageBox(QString::fromLocal8Bit("参数不完整"),
				QString::fromLocal8Bit("请输入断层坐标"));
			return;
		}
	}
	else if (ui.ct3MultiLayerComboBox->currentText() == QString::fromLocal8Bit("多层等间距"))
	{
		if (ui.ct3LayerPosEdit->text().size() ==0 |
			ui.ct3LayerNumLineEdit->text().size() == 0 |
			ui.ct3LayerSpaceLineEdit->text().size() == 0)
		{
			messageBox(QString::fromLocal8Bit("参数不完整"),
				QString::fromLocal8Bit("请输入缺少的参数"));
			return;
		}

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

	accept();
}

void ct3AddModifyDialog::on_cancelButton_clicked()
{
	reject();
}

void ct3AddModifyDialog::refreshPosListWidget()
{
	ui.ct3LayerPosListWidget->clear();

	for (auto pos : d_tempItem->LayerPos)
	{
		QListWidgetItem* item = new QListWidgetItem(QString::number(pos, 'f', 2));
		ui.ct3LayerPosListWidget->addItem(item);
	}
}

void ct3AddModifyDialog::on_ct3LayerPosEdit_returnPressed()
{
	if (d_tempItem->MutilayerOrEqualLayer == EQUALLAYER)
		return;

	QString valueText = ui.ct3LayerPosEdit->text();
	bool succeed = false;
	auto pos = valueText.toFloat(&succeed);

	if (!succeed)
	{
		ui.ct3LayerPosEdit->selectAll();
		return;
	}
	
	ui.ct3LayerPosEdit->selectAll();
	auto it = std::find_if(d_tempItem->LayerPos.begin(), d_tempItem->LayerPos.end(),
		[pos](float value) { return std::abs(value - pos) < 0.01; });

	if (it == d_tempItem->LayerPos.end())
	{
		d_tempItem->LayerPos.insert(pos);
		refreshPosListWidget();
	}
	else
	{
		messageBox(QString::fromLocal8Bit("断层坐标不能相同"),
			QString::fromLocal8Bit("请输入不同断层坐标"));
		return;
	}
}

void ct3AddModifyDialog::on_ct3LayerPosListWidget_itemDoubleClicked(QListWidgetItem * _item)
{
	float pos = _item->text().toFloat();
	auto it = std::find_if(d_tempItem->LayerPos.begin(), d_tempItem->LayerPos.end(),
		[pos](float value) { return std::abs(value - pos) < 0.01; });
	d_tempItem->LayerPos.erase(it);
	refreshPosListWidget();
}

void ct3AddModifyDialog::on_ct3MultiLayerComboBox_currentIndexChanged(const QString& _text)
{
	if (_text == QString::fromLocal8Bit("多层等间距"))
	{
		ui.ct3LayerSpaceLineEdit->show();
		ui.ct3LayerNumLineEdit->show();
		ui.ct3SpaceNumLabel->show();
		ui.ct3LayerSpaceLabel->show();
		ui.label_8->setText(QString::fromLocal8Bit("首层位置"));
		ui.ct3LayerPosListWidget->setEnabled(false);
		ui.ct3LayerPosListWidget->clear();
		d_tempItem->MutilayerOrEqualLayer = EQUALLAYER;

	}
	else if (_text == QString::fromLocal8Bit("多层不等间距"))
	{
		ui.ct3LayerSpaceLineEdit->hide();
		ui.ct3LayerNumLineEdit->hide();
		ui.ct3SpaceNumLabel->hide();
		ui.ct3LayerSpaceLabel->hide();
		ui.label_8->setText(QString::fromLocal8Bit("断层位置"));
		ui.ct3LayerPosListWidget->setEnabled(true);
		d_tempItem->MutilayerOrEqualLayer = MULTILAYER;
	}
}

void ct3AddModifyDialog::on_ct3AngleLineEdit_textEdited(const QString& _text)
{
	bool succeed = false;
	auto pos = _text.toFloat(&succeed);

	if (!succeed)
	{
		ui.ct3AngleLineEdit->clear();
		messageBox(QString::fromLocal8Bit("格式错误"), QString::fromLocal8Bit("请输入正确的数据"));
		return;
	}
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
	if(!isInterger(_text))
	{
		ui.ct3LayerSpaceLineEdit->clear();
		messageBox(QString::fromLocal8Bit("格式错误"), QString::fromLocal8Bit("请输入正确的数据"));
		return;
	}
}
void ct3AddModifyDialog::on_ct3LayerNumLineEdit_textEdited(const QString& _text)
{
	if (!isInterger(_text))
	{
		ui.ct3LayerNumLineEdit->clear();
		messageBox(QString::fromLocal8Bit("格式错误"), QString::fromLocal8Bit("请输入正确的数据"));
		return;
	}
}