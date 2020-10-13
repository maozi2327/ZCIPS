#include "stdafx.h"
#include "linedetscanwidget.h"
#include "simotioncontroller.h"
#include "LineDetNetWork.h"
#include "../Public/headers/setupData.h"
#include "ct3scan.h"
#include "ct3templatewidget.h"
#include "linedetairdisposedialog.h"

LineDetScanWidget::LineDetScanWidget(int _rayId, int _lineDetId, 
	const std::vector<ScanMode>& _scanMode, SetupData* _setupData, 
	LineDetNetWork* _lineDetNetWork, ControllerInterface* _controller, QWidget *parent)
	: QWidget(parent)
	, d_rayNum(_rayId), d_detNum(_lineDetId)
	, d_setupData(_setupData), d_lineDetNetWork(_lineDetNetWork), d_controller(_controller)
{
	ui.setupUi(this);
	initiliseControls();

	for (auto& scanMode : _scanMode)
	{
		if (scanMode == ScanMode::CT3_SCAN)
		{
			auto itr = std::find_if(d_setupData->ct3Data.cbegin(), d_setupData->ct3Data.cend(),
				[=](const CT3Data& _ct3Data)
			{	return _ct3Data.Ray == _rayId && _ct3Data.Det == _lineDetId; });

			initiliseCt3Controls(*itr);
			d_ct3Data = *itr;
		}
		else if (scanMode == ScanMode::DR_SCAN)
		{
			auto itr = std::find_if(d_setupData->drScanData.cbegin(), d_setupData->drScanData.cend(),
				[=](const DrScanData& _drData)
			{	return _drData.Ray == _rayId && _drData.Det == _lineDetId; });

			initiliseDrControls(*itr);
		}
		else if (scanMode == ScanMode::CT2_SCAN)
		{
			auto itr = std::find_if(d_setupData->ct2Data.cbegin(), d_setupData->ct2Data.cend(),
				[=](const CT2Data& _ct2Data)
			{	return _ct2Data.Ray == _rayId && _ct2Data.Det == _lineDetId; });

			initiliseCt2Controls(*itr);
		}
	}

	connect(ui.ct3LayerPosLineEdit, &QLineEdit::returnPressed, this, &LineDetScanWidget::on_ct3LayerPosLineEdit_returnd);
	d_timer = new QTimer();
	d_timer->start(1000);
	connect(d_timer, &QTimer::timeout, this, &LineDetScanWidget::updateControlsSlot);
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
void addItemToMatixVieSample(T& _data, QComboBox* _matrix, QComboBox* _view, QComboBox* _sampleTime,
	int _rayNum, int _detNum)
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
		ui.ct3SampleTimeComboBox, d_rayNum, d_detNum);
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("单层"));
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("多层等间距"));
	ui.ct3MultiLayerComboBox->addItem(QString::fromLocal8Bit("多层不等间距"));
	ui.ct3MultiLayerComboBox->setCurrentText(QString::fromLocal8Bit("单层"));
}

void LineDetScanWidget::initiliseCt2Controls(const CT2Data& _data)
{
	addItemToMatixVieSample(_data, ui.ct2MatrixComboBox, ui.ct2ViewComboBox,
		ui.ct2SampleTimeComboBox, d_rayNum, d_detNum);
	ui.ct2ScanModeComboBox->addItem(QString::fromLocal8Bit("360度"));
	ui.ct2ScanModeComboBox->addItem(QString::fromLocal8Bit("360度间隔"));
	ui.ct2ScanModeComboBox->addItem(QString::fromLocal8Bit("180度"));
}

void LineDetScanWidget::initiliseDrControls(const DrScanData& _data)
{
	addItemToMatixVieSample(_data, ui.drMatrixComboBox, ui.drViewComboBox,
		ui.drSampleTimeComboBox, d_rayNum, d_detNum);
	ui.drRatioComboBox->addItem(QString::fromLocal8Bit("等比例"));
	ui.drRatioComboBox->addItem(QString::fromLocal8Bit("不等比例"));
}


//不判断是否能进行扫描，自动判断设备状态使能和禁止扫描按钮，
//只要按钮有效则都可以进行扫描
void LineDetScanWidget::on_Ct3StartButton_clicked()
{
	float layer;

	if (ui.ct3MultiLayerComboBox->currentText() == QString::fromLocal8Bit("单层"))
		layer = ui.ct3LayerPosLineEdit->text().toFloat();
	else
		layer = ui.ct3LayerPosListWidget->item(0)->text().toFloat();

	int matrix = ui.ct3MatrixComboBox->currentText().toInt();
	int view = ui.ct3ViewComboBox->currentText().toInt();
	int sampleTime = ui.ct3SampleTimeComboBox->currentText().toInt();
	float angle = ui.ct3AngleLineEdit->text().toFloat();
	d_scan.reset(new CT3Scan(d_controller, d_lineDetNetWork, d_setupData, d_detNum));
	
	if (!dynamic_cast<CT3Scan*>(d_scan.get())
		->setScanParameter(layer, matrix, view, sampleTime, angle))
		return;

	connect(d_scan.get(), &CT3Scan::signalGraduationCount, this, &LineDetScanWidget::updateCT3Progresser);
	d_scan->beginScan();
}

void LineDetScanWidget::on_saveDirButton_clicked()
{
	auto srcDirPath = QFileDialog::getExistingDirectory(this, "choose src Directory", "/");
}

void LineDetScanWidget::on_ct3MultiLayerComboBox_currentIndexChanged(const QString& _text)
{
	if (_text == QString::fromLocal8Bit("单层"))
	{
		ui.ct3LayerPosLineEdit->show();
		ui.ct3LayerSpaceLineEdit->hide();
		ui.ct3SpaceNumLineEdit->hide();
		ui.ct3SpaceNumLabel->hide();
		ui.ct3LayerSpaceLabel->hide();
	}
	else if (_text == QString::fromLocal8Bit("多层等间距"))
	{
		ui.ct3LayerPosLineEdit->show();
		ui.ct3LayerSpaceLineEdit->show();
		ui.ct3SpaceNumLineEdit->show();
		ui.ct3SpaceNumLabel->show();
		ui.ct3LayerSpaceLabel->show();
	}
	else if (_text == QString::fromLocal8Bit("多层不等间距"))
	{
		ui.ct3LayerPosLineEdit->show();
		ui.ct3LayerSpaceLineEdit->hide();
		ui.ct3SpaceNumLineEdit->hide();
		ui.ct3SpaceNumLabel->hide();
		ui.ct3LayerSpaceLabel->hide();
	}
}

void LineDetScanWidget::on_ct3LayerPosLineEdit_returnd()
{
	QString valueText = ui.ct3LayerPosLineEdit->text();
	bool succeed = false;
	auto value = valueText.toDouble(&succeed);

	if (!succeed)
		return;

	d_controller->sliceMove(value);
}

void LineDetScanWidget::on_stopButton_clicked()
{
	if(d_scan)
		d_scan->stopScan();

	d_controller->stopAll();
}

void LineDetScanWidget::on_ct3LoadTemplateButton_clicked()
{
	CT3TemplateWidget ct3TemplateWidget(d_ct3Data, this);
	ct3TemplateWidget.exec();
}

void LineDetScanWidget::on_airTuneButton_clicked()
{
	d_airDisposeDialog = LineDetAirDisposeDialog::getInstance(d_controller, d_lineDetNetWork, d_setupData,
		d_rayNum, d_detNum, QString(""), this);
	d_airDisposeDialog->show();
}

void LineDetScanWidget::updateControlsSlot()
{
	bool controlConnected = d_controller->getConnected();
	bool idle = d_controller->readIdleStatus();
	bool ready = d_controller->readReadyStatus();
	bool detConnected = d_lineDetNetWork->getConnected();
	bool readyToScan = controlConnected && idle && ready && detConnected;
	ui.Ct3StartButton->setEnabled(readyToScan);
	QString tip;

	if(!controlConnected)
		tip += QString::fromLocal8Bit("控制系统未连接！\n");
	else
	{
		if (!ready)
			tip += QString::fromLocal8Bit("控制系统未初始化！\n");
		if (!idle)
			tip += QString::fromLocal8Bit("系统忙！\n");
	}

	if (!detConnected)
		tip += QString::fromLocal8Bit("探测器未连接！\n");

	if (!readyToScan)
	{
		ui.Ct3StartButton->setToolTip(tip);
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

void LineDetScanWidget::updateCT3Progresser(int _progress)
{
	ui.Ct3ScanNowProgressBar->setValue(_progress);
}

void LineDetScanWidget::showMotorTable()
{
	
}
