#pragma once

#include <QSharedPointer>
#include <QtCharts>
#include <QWidget>
#include "TripleSlider.h"
#include "MarcoPipeArea.h"
#include "FastBtnArea.h"


// �Ҷȱ任����
class GrayTransUI
{	
public:
	QChart *pChartGray;
	QChartView *pChartViewGray;
	QDoubleSpinBox *pSpinBottom;
	QDoubleSpinBox *pSpinR;
	QDoubleSpinBox *pSpinTop;
	TripleSlider *pTSlider;
	QPushButton *pButtonOK;
	QPushButton *pButtonCancel;
};

// �����
class MarcoUI
{
public:
	MarcoPipeArea *pMarcoPipeArea;
	FastBtnArea *pFastBtnArea;
	QPushButton *pBtnMarcoLoad;
	QPushButton *pBtnMarcoGenerate;
	QPushButton *pBtnPipeCut;
	QPushButton *pBtnMarcoClear;
	QPushButton *pBtnFastEdit;
	QGridLayout *pBtnLayout;
	QVector<QTableWidget *> vecPipleWidget;
	QVector<QPushButton *> vecFastButton;
};