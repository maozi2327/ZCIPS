#include "stdafx.h"
#include "linedetscanwidget.h"
#include "CT3TemplateDialog.h"
#include "linedetairdisposedialog.h"
#include "scantemplate.h"

LineDetScanWidget::LineDetScanWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initiliseControls();
	setFixedHeight(470);
}

LineDetScanWidget::~LineDetScanWidget()
{

}

void LineDetScanWidget::onNetworkStsChanged(bool _netWorkSts)
{
	//ui.startButton->setEnabled(_netWorkSts);
	//ui.startButton->setEnabled(_netWorkSts);
	//ui.scanModeTab->setEnabled(_netWorkSts);
}

void LineDetScanWidget::contextMenuEvent(QContextMenuEvent * event)
{
	showMotorTableAction = new QAction(tr("&paste"), this);
	connect(showMotorTableAction, &QAction::triggered, this, &LineDetScanWidget::showMotorTable);
	QMenu menu(this);
	menu.addAction(showMotorTableAction);
	menu.exec(event->globalPos());
}

//void LineDetScanWidget::keyPressEvent(QKeyEvent * _event)
//{
//	switch (_event->key())
//	{
//	case Qt::Key_Return:
//		QString valueText = ui.ct3LayerPosLineEdit->text();
//		bool succeed = false;
//		auto value = valueText.toDouble(&succeed);
//
//		if (!succeed)
//			return;
//		
//		d_controller->sliceMove(value);
//		break;
//	}
//}

void LineDetScanWidget::disableAllControls()
{

}

template<typename T>
void addItemToMatixVieSample(T& _data, QComboBox* _matrix, QComboBox* _view, QComboBox* _sampleTime)
{
	//for(auto& matrixItr : _data)
	//	if (matrixItr != _data.end())
	//	{
	for (auto& value : _data.Matrix)
		_matrix->addItem(QString::number(value));

	for (auto& value : _data.View)
		_view->addItem(QString::number(value));

	for (auto& value : _data.SampleTime)
		_sampleTime->addItem(QString::number(value));
		//}
}

void LineDetScanWidget::initiliseControls()
{
	ui.ct3LayerPosListWidget->setSortingEnabled(true);
	ui.scanModeTab->setCurrentWidget(ui.ct3Tab);
}

void LineDetScanWidget::initiliseCt3Controls(const CT3Data& _data)
{
	addItemToMatixVieSample(_data, ui.ct3MatrixComboBox, ui.ct3ViewComboBox,
		ui.ct3SampleTimeComboBox);
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("多层等间距"));
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("多层不等间距"));
}

void LineDetScanWidget::initiliseCt2Controls(const CT2Data& _data)
{
	addItemToMatixVieSample(_data, ui.ct2MatrixComboBox, ui.ct2ViewComboBox,
		ui.ct2SampleTimeComboBox);
	ui.ct2ScanModeComboBox->addItem(QString::fromLocal8Bit("360度"));
	ui.ct2ScanModeComboBox->addItem(QString::fromLocal8Bit("360度间隔"));
	ui.ct2ScanModeComboBox->addItem(QString::fromLocal8Bit("180度"));
}

void LineDetScanWidget::initiliseDrControls(const DrScanData& _data)
{
	addItemToMatixVieSample(_data, ui.drMatrixComboBox, ui.drViewComboBox,
		ui.drSampleTimeComboBox);
	ui.drRatioComboBox->addItem(QString::fromLocal8Bit("等比例"));
	ui.drRatioComboBox->addItem(QString::fromLocal8Bit("不等比例"));
}
float LineDetScanWidget::getLayer()
{
	float layer = 0;

	if (ui.ct3MultiLayerComboBox->currentText() == QString::fromLocal8Bit("单层"))
		layer = ui.ct3LayerPosLineEdit->text().toFloat();
	else
		layer = ui.ct3LayerPosListWidget->item(0)->text().toFloat();

	return layer;
}
void LineDetScanWidget::enableScan(bool _sts)
{
	ui.Ct3StartButton->setEnabled(_sts);

	if (!_sts)
	{
		QString style
		(
			"background-color: rgb(255, 170, 127);"
			"border-radius:3px; "
		);
		ui.Ct3StartButton->setStyleSheet(style);
	}
	else
		ui.Ct3StartButton->setStyleSheet("");

}
void LineDetScanWidget::setScanButtonToolTip(const QString & _tip)
{
	ui.Ct3StartButton->setToolTip(_tip);
}
void LineDetScanWidget::switchCt3MultilayerEquallayerShowHide(unsigned char _mode)
{
	if (_mode == MULTILAYER)
	{
		ui.ct3LayerPosListWidget->show();
		ui.ct3LayerSpaceLabel->hide();
		ui.ct3LayerSpaceLineEdit->hide();
		ui.ct3LayerNumLabel->hide();
		ui.ct3LayerNumLineEdit->hide();
		ui.ct3LayerPosListWidget->show();
		ui.label_8->setText(QString::fromLocal8Bit("断层位置"));
	}
	else if (_mode == EQUALLAYER)
	{
		ui.ct3LayerSpaceLabel->show();
		ui.ct3LayerSpaceLineEdit->show();
		ui.ct3LayerNumLabel->show();
		ui.ct3LayerNumLineEdit->show();
		ui.ct3LayerPosListWidget->hide();
		ui.label_8->setText(QString::fromLocal8Bit("首层位置"));
	}
}

void LineDetScanWidget::useCt3Item(const Ct3TemplateData& _templateData)
{
	if (_templateData.MutilayerOrEqualLayer == MULTILAYER)
	{
		switchCt3MultilayerEquallayerShowHide(MULTILAYER);
		ui.ct3LayerPosListWidget->clear();

		for (auto pos : _templateData.LayerPos)
		{
			QListWidgetItem* item = new QListWidgetItem(QString::number(pos, 'f', 2));
			ui.ct3LayerPosListWidget->addItem(item);
		}
	}
	else if (_templateData.MutilayerOrEqualLayer == EQUALLAYER)
	{
		switchCt3MultilayerEquallayerShowHide(EQUALLAYER);
		ui.ct3LayerSpaceLineEdit->setText(QString::number(_templateData.layerSpace, 'f', 2));
		ui.ct3LayerNumLineEdit->setText(QString::number(_templateData.LayerNumber));
		ui.ct3LayerPosLineEdit->setText(QString::number(*_templateData.LayerPos.begin()));
	}

	ui.ct3MatrixComboBox->setCurrentText(QString::number(_templateData.Matrix));
	ui.ct3ViewComboBox->setCurrentText(QString::number(_templateData.View));
	ui.ct3SampleTimeComboBox->setCurrentText(QString::number(_templateData.SampleTime));
	ui.ct3AngleLineEdit->setText(QString::number(_templateData.Orientation, 'f', 2));
}

//不判断是否能进行扫描，自动判断设备状态使能和禁止扫描按钮，
//只要按钮有效则都可以进行扫描
void LineDetScanWidget::on_Ct3StartButton_clicked()
{
	emit ct3ScanSignal();
}

void LineDetScanWidget::on_saveDirButton_clicked()
{
	auto srcDirPath = QFileDialog::getExistingDirectory(this, "choose src Directory", "/");
}

void LineDetScanWidget::on_ct3MultiLayerComboBox_currentIndexChanged(const QString& _text)
{
	if (_text == QString::fromLocal8Bit("多层等间距"))
		switchCt3MultilayerEquallayerShowHide(MULTILAYER);
	else if (_text == QString::fromLocal8Bit("多层不等间距"))
		switchCt3MultilayerEquallayerShowHide(EQUALLAYER);
}

void LineDetScanWidget::on_ct3LayerPosLineEdit_returnd()
{
	//QString valueText = ui.ct3LayerPosLineEdit->text();
	//bool succeed = false;
	//auto value = valueText.toDouble(&succeed);

	//if (!succeed)
	//	return;

	//d_controller->sliceMove(value);
}

void LineDetScanWidget::on_stopButton_clicked()
{
	emit stopButtonSignal();
}

void LineDetScanWidget::on_ct3LoadTemplateButton_clicked()
{
	emit LoadCt3TemplateButtonSignal();
}

void LineDetScanWidget::on_bkgTuneButton_clicked()
{
	emit bkgTuneSignal();
}

void LineDetScanWidget::on_loadTuneDataButton_clicked()
{
	emit loadTuneDataSignal();
}

void LineDetScanWidget::on_airTuneButton_clicked()
{
	emit airTuneSignal();
}

void LineDetScanWidget::updateControlsSlot()
{

}

void LineDetScanWidget::updateCT3Progresser(int _progress)
{
	ui.Ct3ScanNowProgressBar->setValue(_progress);
}

void LineDetScanWidget::showMotorTable()
{
	
}
