#pragma once
#include "ctscan_global.h"
#include <QObject>

class CTScanInterface : public QObject
{
public slots:
	void slot1(int i);

signals:
	void signal1(int i);
};

class CTSCANDLL_EXPORT CtScanExport
{
public:
	CtScanExport() = delete;
	~CtScanExport();

	static QObject* loadCtScanWidget(QWidget* _parent);
};

//TODO_DJ_NOTEPAD
//使用unique_ptr包裹QDialog，不会自动析构;
//ctscanwidget不能响应closeevent;
