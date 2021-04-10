#include "stdafx.h"
#include "LineDetImageProcess.h"
#include "../../Public/util/functions.h"
#include <regex>

LineDetImageProcess::LineDetImageProcess(const QString& _workDirectory)
	: d_workDirectory(_workDirectory)
{

}

LineDetImageProcess::~LineDetImageProcess()
{

}

int LineDetImageProcess::saveOrgFile(const QString& _destFile, void * pHeader, void * pListItem, int iPw)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	return saveOrgHeaderAndList(destFileName.data(), pHeader, pListItem, iPw);
}

int LineDetImageProcess::createAirDat(const QString& _sourceFile, const QString& _destPath)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	int ret = fCreateAirRatio(d_workDirectory.toLocal8Bit().data(), sourceFileName.data());

	if (ret == 0)
	{
		std::regex reg("[^/]+(?=\\.)");
		std::smatch result;
		std::string fileName = _sourceFile.toLocal8Bit();
		std::regex_search(fileName, result, reg);
		fileName = result.str();
		QString pureFileName = fileName.c_str();
		QFile::copy(d_workDirectory + pureFileName + QString::fromLocal8Bit(".dat"), 
			_destPath + pureFileName + QString::fromLocal8Bit(".dat"));
		QFile::rename(d_workDirectory + pureFileName + QString::fromLocal8Bit(".dat"),
			d_workDirectory + QString::fromLocal8Bit("air.dat"));
		return 0;
	}

	return ret;
}

int LineDetImageProcess::createBkgDat(const QString& _sourceFile, const QString& _destPath)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	int ret = fCreateBkgData(d_workDirectory.toLocal8Bit().data(), sourceFileName.data());

	if (ret == 0)
	{
		std::regex reg("[^/]+(?=\\.)");
		std::smatch result;
		std::string fileName = _sourceFile.toLocal8Bit();
		std::regex_search(fileName, result, reg);
		fileName = result.str();
		QString pureFileName = fileName.c_str();
		QFile::copy(d_workDirectory + pureFileName + QString::fromLocal8Bit(".dat"),
			_destPath + pureFileName + QString::fromLocal8Bit(".dat"));
		QFile::rename(d_workDirectory + pureFileName + QString::fromLocal8Bit(".dat"),
			d_workDirectory + QString::fromLocal8Bit("bkg.dat"));
		return 0;
	}

	return ret;
}

int LineDetImageProcess::ct3Tune(const QString & _orgFile, const QString& _destPath)
{
	return generalTune(_orgFile, _destPath);
}

int LineDetImageProcess::drTune(const QString& _orgFile, const QString& _destPath)
{
	return generalTune(_orgFile, _destPath);
}

int LineDetImageProcess::generalTune(const QString& _orgFile, const QString& _destPath)
{
	QByteArray orgFileName = replaceSlashToBslash(_orgFile).toLocal8Bit();
	QByteArray destDirectoryByte = replaceSlashToBslash(_destPath).toLocal8Bit();
	QByteArray workDirectory = replaceSlashToBslash(d_workDirectory).toLocal8Bit();
	unsigned short setupWord = EN_BKGTUNE | EN_AIRTUNE | EN_RANGE | EN_CROSSTALK | EN_PULSEDOSE | EN_HARDEN | EN_ABNORMALDETECTOR | EN_SENSITIVE;
	char szFullPathDesFileName[MAX_PATH];							//全路径目的文件名
	return fDataDispose(workDirectory.data(), orgFileName.data(), destDirectoryByte.data(), szFullPathDesFileName, setupWord);
}