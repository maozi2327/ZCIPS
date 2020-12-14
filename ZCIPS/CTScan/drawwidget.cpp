#include "stdafx.h"
#include "drawwidget.h"

DrawWidget::DrawWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setPalette(QPalette(Qt::white));
	setAutoFillBackground(true);
	d_graphTop = 10;
	d_graphLeft = 50;
	d_graphRightMargin = 20;
	d_graphDownMargin = 10;
	d_data = new std::vector<int>(608);

	//for (int i = 0; i != 608; ++i)
	//	(*d_data)[i] = i * 10;
}

DrawWidget::~DrawWidget()
{
}

void DrawWidget::drawCurve(QPainter& _painter)
{
	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::red);
	pen.setStyle(Qt::SolidLine);
	pen.setCapStyle(Qt::FlatCap);
	pen.setJoinStyle(Qt::BevelJoin);
	_painter.setPen(pen);

	for (int i = 0; i != 11; ++i)
	{
		QLine line(0, d_graphHeight - d_yLineSpace * i,	d_graphWidth, d_graphHeight - d_yLineSpace * i);
		_painter.drawLine(line);
	}

	for (int i = 0; i != 11; ++i)
	{
		QLine line(i * d_xLineSpace, d_graphHeight, i * d_xLineSpace, 0);
		_painter.drawLine(line);
	}

	QLine line(0, 0, 0, d_graphHeight - (*d_data)[0] / d_yValuePerPixel);

	for(int i = 1; i != (*d_data).size(); ++i)
	{
		QLine line((i - 1) / d_xValuePerPixel, d_graphHeight - (*d_data)[i - 1] / d_yValuePerPixel,
			i / d_xValuePerPixel, d_graphHeight - (*d_data)[i] / d_yValuePerPixel);

		QPen pen;
		pen.setWidth(2);
		pen.setColor(Qt::blue);
		pen.setStyle(Qt::SolidLine);
		pen.setCapStyle(Qt::FlatCap);
		pen.setJoinStyle(Qt::BevelJoin);
		_painter.setPen(pen);
		_painter.drawLine(line);
	}
}

void DrawWidget::paintEvent(QPaintEvent * event)
{
	QPixmap pixmap;
	auto itr = std::max_element(d_data->begin(), d_data->end());
	int max = *itr / 10 * 10;
	auto widgetWidth = width();
	auto widgetHeight = height();
	d_graphWidth = widgetWidth - d_graphLeft - d_graphRightMargin;
	d_graphHeight = widgetHeight - d_graphTop - d_graphDownMargin;
	d_yValuePerPixel = float(max) / d_graphHeight;
	d_xValuePerPixel = float(d_data->size() + 1) / d_graphWidth;
	d_xLineSpace = float(d_graphWidth) / 10;
	d_yLineSpace = float(d_graphHeight) / 10;
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::TextAntialiasing);

	//int w = width();
	//int h = height();
	//QRect rect(w / 4, h / 4, w / 2, h / 2);
	//QBrush brush;
	//brush.setColor(Qt::yellow);
	//brush.setStyle(Qt::SolidPattern);
	//painter.setBrush(brush);
	//painter.drawRect(rect);
	QFont font;
	font.setPointSize(10);
	font.setBold(true);
	painter.setFont(font);

	//»­ºá×ø±ê
	for (int i = 0; i != 11; ++i)
	{
		QPoint point(i * d_xLineSpace + d_graphLeft, widgetHeight - d_graphDownMargin + 10);
		painter.drawText(point, QString("%1").arg(i * d_data->size() / 10));
	}

	painter.setViewport(d_graphLeft, d_graphTop, widgetWidth, widgetHeight);
	drawCurve(painter);
}

void DrawWidget::draw(unsigned long * _data, int _size)
{
	std::copy(_data, _data + _size, d_data->begin());
	update();
	//memcpy(d_data, _data, _size * sizeof(unsigned long));
}
