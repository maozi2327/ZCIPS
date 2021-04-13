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

	//�����ļ��������һ���»��ߺ���������Ǳ����ļ���
	QString tunedFileName = d_filePath + getPureFileNameFromFullName(d_orgName) + QString::fromLocal8Bit(".tif");
	std::vector<QString> allAirFiles;
	browseAllFiles(d_filePath, allAirFiles);

	//��������Ŀ����ļ��Ŀ�������ͬ���Ͱ�������ɾ������������ؿ�������ʱ��ʾͬ����ͬ���ļ�����
	for (auto& file : allAirFiles)
	{
		QString pureFileName = getPureFileNameFromFullName(file);

		if (pureFileName.left(pureFileName.indexOf(QChar('_'))) == getPureFileNameFromFullName(d_orgName).left(pureFileName.indexOf(QChar('_'))))
			QFile::remove(file);
	}

	d_imageProcess->mirrorYDataToData(d_outputBuff, d_height, d_width);

	if (d_saveOrg)
		d_imageProcess->saveSingleBitmapDataToFile(d_outputBuff, d_orgName, d_width, d_height);

	d_imageProcess->createAirDataToFile(&d_outputBuff, tunedFileName, d_bkgFileName, d_width, d_height, 1);
}

//TODO_DJ���������ɼ�״̬����������ƬԤ���������õȰ�ť