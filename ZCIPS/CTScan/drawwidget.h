#pragma once

#include <QWidget>
#include "ui_drawwidget.h"
#include <vector>

class DrawWidget : public QWidget
{
	Q_OBJECT

public:
	DrawWidget(QWidget *parent = Q_NULLPTR);
	~DrawWidget();

private:
	Ui::DrawWidget ui;
	std::vector<int>* d_data;
	int d_graphWidth;
	int d_graphHeight;
	int d_graphTop;
	int d_graphLeft;
	int d_graphDownMargin;
	int d_graphRightMargin;
	float d_xLineSpace;
	float d_yLineSpace;
	void drawCurve(QPainter& pen);
	float d_yValuePerPixel;
	float d_xValuePerPixel;
protected:
	virtual void paintEvent(QPaintEvent *event) override;
};
