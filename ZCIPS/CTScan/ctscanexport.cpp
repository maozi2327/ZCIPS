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

QObject* CtScanExport::loadCtScanWidget(QWidget* _parent)
{
	static std::unique_ptr<CTScanInterface> handle(new CTScanApp(_parent));
	return handle.get();
}
