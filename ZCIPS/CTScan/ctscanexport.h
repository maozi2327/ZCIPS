#pragma once
#include "ctscan_global.h"
#include <QObject>
#include <memory>
struct Member;

class CTSCANDLL_EXPORT CtScanExport
{
private:
	std::unique_ptr<Member> pimp;
public:
	CtScanExport(QWidget* in_parent);
	~CtScanExport();

	static QWidget* loadCtScanWidget(QWidget* in_parent);
};

