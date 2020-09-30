#include "stdafx.h"
#include "ct3AddModifyDialog.h"
#include "../Public/headers/SetupData.h"
#include "ct3templatewidget.h"

ct3AddModifyDialog::ct3AddModifyDialog(std::vector<Ct3TemplateData>& _tempData, 
	std::vector<Ct3TemplateData>::iterator _itr, const CT3Data& _ct3Data, QWidget *parent)
	: QDialog(parent), d_itr(_itr)
{
	ui.setupUi(this); 	
	
	if (d_itr == _tempData.end())
	{
		Ct3TemplateData data;
		_tempData.push_back(data);
		d_itr = _tempData.end() - 1;
	}

	initiliseCt3Controls(_ct3Data);	
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
	d_itr->Matrix = ui.ct3MatrixComboBox->currentText().toInt();
}

void ct3AddModifyDialog::on_ct3ViewComboBox_currentIndexChanged(const QString & _text)
{
	d_itr->View = ui.ct3ViewComboBox->currentText().toInt();
}

void ct3AddModifyDialog::on_ct3SampleTimeComboBox_currentIndexChanged(const QString & _text)
{
	d_itr->SampleTime = ui.ct3SampleTimeComboBox->currentText().toInt();
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

void ct3AddModifyDialog::on_okButton_Clicked()
{
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

		d_itr->LayerPos.insert(ui.ct3LayerPosEdit->text().toFloat());
		d_itr->ecqualLayerNumber = ui.ct3LayerNumLineEdit->text().toFloat();
		d_itr->layerSpace = ui.ct3LayerSpaceLineEdit->text().toFloat();
	}

	//d_itr->Matrix = ui.ct3MatrixComboBox->currentText().toInt();
	//d_itr->View = ui.ct3ViewComboBox->currentText().toInt();
	//d_itr->SampleTime = ui.ct3SampleTimeComboBox->currentText().toInt();
}

void ct3AddModifyDialog::refreshPosListWidget()
{
	ui.ct3LayerPosListWidget->clear();

	for (auto pos : d_itr->LayerPos)
	{
		QListWidgetItem* item = new QListWidgetItem(QString::number(pos, 'f', 2));
		ui.ct3LayerPosListWidget->addItem(item);
	}
}

void ct3AddModifyDialog::on_ct3LayerPosEdit_returnPressed()
{
	if (d_itr->MutilayerOrEqualLayer = EQUALLAYER)
		return;

	QString valueText = ui.ct3LayerPosEdit->text();
	bool succeed = false;
	auto value = valueText.toFloat(&succeed);

	if (!succeed)
	{
		ui.ct3LayerPosEdit->selectAll();
		return;
	}

	if (d_itr->LayerPos.find(value) == d_itr->LayerPos.end())
	{
		d_itr->LayerPos.insert(value);
		refreshPosListWidget();
	}

	ui.ct3LayerPosEdit->selectAll();
}

void ct3AddModifyDialog::on_ct3LayerPosListWidget_itemDoubleClicked(QListWidgetItem * _item)
{
	float pos = _item->text().toFloat();
	auto it = std::find_if(d_itr->LayerPos.begin(), d_itr->LayerPos.end(),
		[pos](float value) { return std::abs(value - pos) < 0.01; });
	d_itr->LayerPos.erase(it);
	refreshPosListWidget();
}

void ct3AddModifyDialog::on_ct3MultiLayerComboBox_currentIndexChanged(const QString& _text)
{
	if (_text == QString::fromLocal8Bit("多层等间距"))
	{
		//ui.ct3LayerSpaceLineEdit->show();
		//ui.ct3LayerNumLineEdit->show();
		//ui.ct3SpaceNumLabel->show();
		//ui.ct3LayerSpaceLabel->show();
		ui.label_8->setText(QString::fromLocal8Bit("首层位置"));
		ui.ct3LayerPosListWidget->setEnabled(false);
		ui.ct3LayerPosListWidget->clear();
		d_itr->MutilayerOrEqualLayer = EQUALLAYER;
	}
	else if (_text == QString::fromLocal8Bit("多层不等间距"))
	{
		//ui.ct3LayerSpaceLineEdit->hide();
		//ui.ct3LayerNumLineEdit->hide();
		//ui.ct3SpaceNumLabel->hide();
		//ui.ct3LayerSpaceLabel->hide();
		ui.label_8->setText(QString::fromLocal8Bit("断层位置"));
		ui.ct3LayerPosListWidget->setEnabled(true);
		d_itr->MutilayerOrEqualLayer = MULTILAYER;
	}
}