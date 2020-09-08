#include "stdafx.h"
#include "ctscanexport.h"
#include "ctscan.h"

CtScanExport::CtScanExport(QWidget* in_parent)
{

}


CtScanExport::~CtScanExport()
{
}

void CtScanExport::loadCtScanWidget(QWidget* in_parent)
{
	static std::unique_ptr<CTScan> handle(std::make_unique<CTScan>(in_parent));
	//new CTScan(in_parent);
}
