#include "stdafx.h"
#include "panelairtune.h"
#include "../Public/util/functions.h"
#include "../PanelImageProcess/panelimageprocess.h"


PanelAirTune::PanelAirTune(Panel* _panel, PanelImageProcess* _panelImageProcess, QObject *parent)
	: PanelFrameSamleInterface(_panel, _panelImageProcess, parent)
{

}


PanelAirTune::~PanelAirTune()
{

}

void PanelAirTune::setBkgFileName(const QString & _bkgFileName)
{
	d_bkgFileName = _bkgFileName;
}

void PanelAirTune::saveFile()
{
	QDir  dir;
	QString orgPath = d_orgName.left(d_orgName.lastIndexOf('/') + 1);

	if (!dir.exists(orgPath))
		dir.mkpath(d_orgName.left(d_orgName.lastIndexOf('/') + 1));

	if (!dir.exists(d_filePath))
		dir.mkpath(d_filePath);

	//空气文件名称最后一个下划线后面的名字是背景文件名
	QString tunedFileName = d_filePath + getPureFileNameFromFullName(d_orgName) + QString::fromLocal8Bit(".tif");
	d_imageProcess->mirrorYDataToData(d_outputBuff, d_height, d_width);

	if (d_saveOrg)
		d_imageProcess->saveSingleBitmapDataToFile(d_outputBuff, d_orgName, d_width, d_height);

	d_imageProcess->createAirDataToFile(&d_outputBuff, tunedFileName, d_bkgFileName, d_width, d_height, 1);
}
