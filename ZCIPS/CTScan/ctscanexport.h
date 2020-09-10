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

