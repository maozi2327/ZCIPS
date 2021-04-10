#include "stdafx.h"
#include "panelframeshot.h"
#include "../PanelDetector/panel.h"
#include "../PanelImageProcess/panelimageprocess.h"
#include "../public/util/functions.h"


//保存 工件名称，编号，日期，时间，宽度，高度到dicom文件
PanelFrameShot::PanelFrameShot(Panel* _panel, PanelImageProcess* _panelImageProcess, bool _bkgTuneFlag,
	bool _airTuneFlag, bool _defectTuneFlag, QObject *parent)
	: PanelFrameSamleInterface(_panel, _panelImageProcess, parent)
	, d_airTuneFlag(_airTuneFlag), d_bkgTuneFlag(_bkgTuneFlag), d_defectTuneFlag(_defectTuneFlag)
{
}

PanelFrameShot::~PanelFrameShot()
{
}


void PanelFrameShot::beginPreview(int _cycleTime, unsigned short _gainFactor)
{
	std::function<void(unsigned short*)> callBack = std::bind(&PanelFrameShot::previewCallback, this, std::placeholders::_1);
	//d_panel->setFrameCallback(callBack);
	d_panel->beginPreview(callBack, _cycleTime, _gainFactor);
}

void PanelFrameShot::setDicomInfo(const QString & _objectName, const QString & _objectNumber)
{
	d_objectName = _objectName;
	d_objectNumber = _objectNumber;
}

void PanelFrameShot::previewCallback(unsigned short * _image)
{
}

void PanelFrameShot::saveFile()
{
	QDir  dir;
	QString orgPath = d_orgName.left(d_orgName.lastIndexOf('/') + 1);

	if (!dir.exists(orgPath))
		dir.mkpath(d_orgName.left(d_orgName.lastIndexOf('/') + 1));

	if (!dir.exists(d_filePath))
		dir.mkpath(d_filePath);

	QString timeName = getTimeWithUnderline();
	d_orgName = orgPath + timeName + d_orgName.right(d_orgName.length() - d_orgName.lastIndexOf('_'));
	QString tunedFileName = d_filePath + timeName + d_orgName.right(d_orgName.length() - d_orgName.lastIndexOf('_'));

	//TODO_DJ：添加旋转或者对称标志到配置文件
	d_imageProcess->mirrorYDataToData(d_outputBuff, d_height, d_width);
	//d_orgName = QString::fromLocal8Bit("D:\\image-000002.dcm");

	if (d_saveOrg)
		//d_imageProcess->saveSingleBitmapDataToFile(d_outputBuff, d_orgName, d_height, d_width);
		d_imageProcess->saveSingleBitmapDataToDicomFile(d_outputBuff, d_orgName, d_height, d_width, d_objectName, d_objectNumber);
	
	if (d_defectTuneFlag)
		d_imageProcess->bkgAirDefectTuneDataToDicomFile(d_outputBuff, tunedFileName, d_height, d_width, d_objectName, d_objectNumber);
		//d_imageProcess->bkgAirDefectTuneDataToFile(d_outputBuff, tunedFileName, d_height, d_width);
	else if (d_airTuneFlag)
		d_imageProcess->bkgAirTuneDataToFile(d_outputBuff, tunedFileName, d_height, d_width);
	else
	{
		if (d_saveOrg)
			QFile::copy(d_orgName, tunedFileName);
		else
			d_imageProcess->saveSingleBitmapDataToFile(d_outputBuff, tunedFileName, d_height, d_width);
	}
}
