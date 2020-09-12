#include "stdafx.h"
#include "floatacceleratordialog.h"

FloatAcceleratorDialog::FloatAcceleratorDialog(QWidget* parent, Qt::WindowType t)
	: QDialog(parent, t)
{
}

FloatAcceleratorDialog::~FloatAcceleratorDialog()
{
}

void FloatAcceleratorDialog::mousePressEvent(QMouseEvent * event)
{
	d_mousePosX = event->x();
	d_mousePosY = event->y();
}

void FloatAcceleratorDialog::mouseMoveEvent(QMouseEvent * event)
{
	move(event->globalX() - d_mousePosX, event->globalY() - d_mousePosY);
}
