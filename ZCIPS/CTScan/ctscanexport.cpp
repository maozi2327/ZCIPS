#include "stdafx.h"
#include "ctscanexport.h"
#include "ctscan.h"

struct Member
{
public:
	CTScan d_ctScan;
	Member(QWidget *in_parent) : d_ctScan(in_parent)
	{

	}
};


CtScanExport::CtScanExport(QWidget* in_parent)
{
	pimp.reset(new Member(in_parent));
}


CtScanExport::~CtScanExport()
{
}

QWidget * CtScanExport::loadCtScanWidget(QWidget* in_parent)
{
	static std::unique_ptr<CTScan> handle(std::make_unique<CTScan>(in_parent));
	return handle.get()->getWidget();
}
