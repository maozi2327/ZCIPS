#pragma once
#include "ctscan_global.h"
#include <QObject>
#include <memory>

class CTSCANDLL_EXPORT CtScanExport
{
public:
	CtScanExport(QWidget* in_parent);
	~CtScanExport();

	static QObject* loadCtScanWidget(QWidget* in_parent);
};

