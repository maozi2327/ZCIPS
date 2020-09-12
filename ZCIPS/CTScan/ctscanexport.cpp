#include "stdafx.h"
#include "ctscanexport.h"
#include "ctscanapp.h"
#include <memory>

void CTScanInterface::slot1(int i)
{
}

CTScanInterface::CTScanInterface()
{
}

CTScanInterface::~CTScanInterface()
{
}

void CTScanInterface::ctScanWidgetClosed()
{
}

CTScanInterface * CTScanInterface::loadCtScanInstance(QWidget * _parent)
{
	static std::unique_ptr<CTScanApp> handle(new CTScanApp(_parent));
	return handle.get();
}
