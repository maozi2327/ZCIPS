#include "stdafx.h"
#include "ctscanexport.h"
#include "ctscanapp.h"
#include <memory>

void CTScanInterface::slot1(int i)
{
}

CtScanExport::~CtScanExport()
{
}

QObject* CtScanExport::loadCtScanWidget(QWidget* in_parent)
{
	static std::unique_ptr<CTScanInterface> handle(new CTScanApp(in_parent));
	return handle.get();
}
