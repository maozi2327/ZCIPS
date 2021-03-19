#include "stdafx.h"
#include "LineDetImageProcess.h"


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
		QString pureFileName = _sourceFile.left(
			_sourceFile.right(_sourceFile.length() - _sourceFile.lastIndexOf('/') - 1).lastIndexOf('.'));
		QFile::copy(d_workDirectory + pureFileName + QString::fromLocal8Bit(".dat"), 
			_destPath + pureFileName + QString::fromLocal8Bit(".dat"));
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
		QString pureFileName = _sourceFile.left(
			_sourceFile.right(_sourceFile.length() - _sourceFile.lastIndexOf('/') - 1).lastIndexOf('.'));
		QFile::copy(d_workDirectory + pureFileName + QString::fromLocal8Bit(".dat"),
			_destPath + pureFileName + QString::fromLocal8Bit(".dat"));
		return 0;
	}

	return ret;
}

int LineDetImageProcess::ct3Dispose(const QString & _orgFile, const QString& _destPath)
{
	QByteArray workDirectory = d_workDirectory.toLocal8Bit();
	QByteArray orgFileName = _orgFile.toLocal8Bit();
	QByteArray destDirectoryByte = _destPath.toLocal8Bit();
	unsigned short setupWord = EN_BKGTUNE | EN_AIRTUNE | EN_SENSITIVE | EN_RANGE | EN_CROSSTALK
		| EN_PULSEDOSE | EN_HARDEN | EN_ABNORMALDETECTOR;

	return fDataDispose(workDirectory.data(), orgFileName.data(), destDirectoryByte.data(), nullptr, setupWord);
}

int LineDetImageProcess::drDispose(const QString & _sourceFile)
{
	return 0;
}
