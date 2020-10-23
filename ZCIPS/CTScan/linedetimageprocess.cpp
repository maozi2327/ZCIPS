#include "stdafx.h"
#include "LineDetImageProcess.h"


LineDetImageProcess::LineDetImageProcess(const QString& _workDirectory, const QString& _orgPath
	, const QString& _ctPath, const QString& _drPath)
	: d_workDirectory(_workDirectory), d_orgPath(_orgPath), d_ctPath(_ctPath), d_drPath(_drPath)
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

int LineDetImageProcess::createAirDat(const QString& _sourceFile)
{
	QString fullDirName = d_orgPath + _sourceFile;
	QByteArray sourceFileName = fullDirName.toLocal8Bit();
	return fCreateAirRatio(d_workDirectory.toLocal8Bit().data(), sourceFileName.data());
}

int LineDetImageProcess::createBkgDat(const QString& _sourceFile)
{
	QString fullDirName = d_orgPath + _sourceFile;
	QByteArray sourceFileName = fullDirName.toLocal8Bit();
	return fCreateBkgData(d_workDirectory.toLocal8Bit().data(), sourceFileName.data());
}

int LineDetImageProcess::dispose(const QString& _sourceFile)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	unsigned short setupWord = EN_BKGTUNE | EN_AIRTUNE | EN_SENSITIVE | EN_RANGE | EN_CROSSTALK 
		| EN_PULSEDOSE | EN_HARDEN | EN_ABNORMALDETECTOR;

	return 0;// fDataDispose(installFolder.data(), sourceFileName.data(), destFileName.data(), nullptr, setupWord);
}