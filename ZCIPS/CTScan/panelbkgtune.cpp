#include "stdafx.h"
#include "panelbkgtune.h"
#include "../PanelDetector/panel.h"
#include "../Public/util/functions.h"
#include "../Public/util/functions.h"
#include "../PanelImageProcess/panelimageprocess.h"

PanelBkgTune::PanelBkgTune(Panel* _panel, PanelImageProcess* _panelImageProcess, QObject *parent)
	: PanelFrameSamleInterface(_panel, _panelImageProcess, parent)
{
}


PanelBkgTune::~PanelBkgTune()
{

}

void PanelBkgTune::saveFile()
{
	QDir  dir;
	QString orgPath = d_orgName.left(d_orgName.lastIndexOf('/') + 1);

	if (!dir.exists(orgPath))
		dir.mkpath(d_orgName.left(d_orgName.lastIndexOf('/') + 1));

	if (!dir.exists(d_filePath))
		dir.mkpath(d_filePath);

	QString tunedFileName = d_filePath + d_orgName.right(d_orgName.length() - d_orgName.lastIndexOf('/') - 1);
	d_imageProcess->mirrorYDataToData(d_outputBuff, d_height, d_width);

	if (d_saveOrg)
		d_imageProcess->saveSingleBitmapDataToFile(d_outputBuff, d_orgName, d_width, d_height);

	d_imageProcess->createBkgDataToFile(&d_outputBuff, tunedFileName, d_width, d_height, 1);
}
