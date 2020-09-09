#include "stdafx.h"
#include "ctscanexport.h"
#include "ctscanapp.h"

CtScanExport::CtScanExport(QWidget* in_parent)
{

}


CtScanExport::~CtScanExport()
{
}

QObject* CtScanExport::loadCtScanWidget(QWidget* in_parent)
{
	static std::unique_ptr<CTScanApp> handle(new CTScanApp(in_parent));
	return handle.get();
}
