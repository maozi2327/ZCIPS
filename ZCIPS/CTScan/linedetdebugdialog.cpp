#include "stdafx.h"
#include "linedetdebugdialog.h"
#include "drawwidget.h"
#include "linedetnetwork.h"
#include "ui_linedetdebugdialog.h"

std::uniform_int_distribution<unsigned> u(0, 500);
std::default_random_engine e;

LineDetDebugDialog::LineDetDebugDialog(LineDetNetWork* _lineDetNetWork, QWidget *parent)
	: QDialog(parent), d_data(nullptr), d_channelNum(608), d_lineDetWork(_lineDetNetWork)
{
	ui.setupUi(this);

	d_graph = new DrawWidget(this);
	d_graph->setObjectName(QStringLiteral("widget"));
	d_graph->setMinimumSize(QSize(800, 600));

	ui.horizontalLayout->addWidget(d_graph);
}

LineDetDebugDialog::~LineDetDebugDialog()
{
	if (d_data != nullptr)
		delete[] d_data;
}

void LineDetDebugDialog::on_downloadSampleTimeButton_clicked()
{
	d_lineDetWork->SetSampleTime(ui.sampleTimeLineEdit->text().toInt());
}

void LineDetDebugDialog::on_downloadDelayTimeButton_clicked()
{
	d_lineDetWork->SetDelayTime(ui.delayTimeLineEdit->text().toInt());
}

void LineDetDebugDialog::on_downloadAmplifyFactorButton_clicked()
{
	d_lineDetWork->SetAmpSize(ui.amplifyFactorLineEdit->text().toInt());
}

void LineDetDebugDialog::on_uploadSampleTimeButton_clicked()
{
	if (d_lineDetWork->ReadSampleTime())
	{
		auto data = d_lineDetWork->getRowList();
		d_graph->draw(data->d_item + 2, d_channelNum);
	}
}

void LineDetDebugDialog::on_uploadAmplifyFactorButton_clicked()
{
	if (d_lineDetWork->ReadAmpSize())
	{
		auto data = d_lineDetWork->getRowList();
		d_graph->draw(data->d_item + 2, d_channelNum);
	}
}

void LineDetDebugDialog::on_uploadDelayTimeButton_clicked()
{
	if (d_lineDetWork->ReadDelayTime())
	{
		auto data = d_lineDetWork->getRowList();
		d_graph->draw(data->d_item + 2, d_channelNum);
	}
}

void LineDetDebugDialog::on_setDepthButton_clicked()
{
	d_lineDetWork->setChannelDepth(ui.depthLineEdit->text().toInt());
}

void LineDetDebugDialog::on_setMaskButton_clicked()
{
	d_lineDetWork->setChannelMask(ui.maskLineEdit->text().toInt());
}


void LineDetDebugDialog::on_internalAcquireButton_clicked()
{
	if (d_data == nullptr)
		d_data = new unsigned long[d_channelNum];

	d_timer = new QTimer(this);
	d_timer->start(20);
	connect(d_timer, &QTimer::timeout, this, &LineDetDebugDialog::updateDataSlot);

	d_TESTtimer = new QTimer(this);
	d_TESTtimer->start(10);
	connect(d_TESTtimer, &QTimer::timeout, this, &LineDetDebugDialog::updateDataSlot1);
	//d_lineDetWork->startInternalTrigAcquire(8, d_data);
}

void LineDetDebugDialog::on_stopInternalAcquireButton_clicked()
{
	d_lineDetWork->stopInternalTrigAcquire();
}

void LineDetDebugDialog::updateDataSlot()
{
	d_graph->draw(d_data, d_channelNum);
}

void LineDetDebugDialog::updateDataSlot1()
{
	auto val = u(e);

	for (int i = 0; i != d_channelNum; ++i)
		d_data[i] = val + i;
}

//void LineDetDebugDialog::paintEvent(QPaintEvent * event)
//{
//	QPainter painter(this);
//	painter.setRenderHint(QPainter::Antialiasing);
//	painter.setRenderHint(QPainter::TextAntialiasing);
//	int w = ui.widget->width();
//	int h = ui.widget->height();
//	QRect rect(w / 4, h / 4, w / 2, h / 2);
//	QPen pen;
//	pen.setWidth(3);
//	pen.setColor(Qt::red);
//	pen.setStyle(Qt::SolidLine);
//	pen.setCapStyle(Qt::FlatCap);
//	pen.setJoinStyle(Qt::BevelJoin);
//	painter.setPen(pen);
//	QBrush brush;
//	brush.setColor(Qt::yellow);
//	brush.setStyle(Qt::SolidPattern);
//	painter.setBrush(brush);
//	painter.drawRect(rect);
//}
