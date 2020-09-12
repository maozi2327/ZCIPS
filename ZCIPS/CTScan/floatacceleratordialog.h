#pragma once

#include <QDialog>

class FloatAcceleratorDialog : public QDialog
{
	Q_OBJECT

public:
	FloatAcceleratorDialog(QWidget *parent = nullptr, Qt::WindowType t = Qt::WindowType());
	~FloatAcceleratorDialog();
private:
	int d_mousePosX;
	int d_mousePosY;
protected:
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
};
