#include "stdafx.h"
#include "siyuan.h"
#include "../CTScan/ctscanexport.h"

SiYuan::SiYuan(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	CTScanInterface::loadCtScanInstance(ui.widget);
}
