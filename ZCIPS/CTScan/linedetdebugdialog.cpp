#include "stdafx.h"
#include "linedetdebugdialog.h"
#include "drawwidget.h"

LineDetDebugDialog::LineDetDebugDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	DrawWidget* widget = new DrawWidget(this);
	widget->setObjectName(QStringLiteral("widget"));
	widget->setMinimumSize(QSize(800, 600));

	ui.horizontalLayout->addWidget(widget);

}

LineDetDebugDialog::~LineDetDebugDialog()
{
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
