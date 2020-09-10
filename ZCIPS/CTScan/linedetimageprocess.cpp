#include "stdafx.h"
#include "LineDetImageProcess.h"


LineDetImageProcess::LineDetImageProcess()
{

}


LineDetImageProcess::~LineDetImageProcess()
{

}

int LineDetImageProcess::saveOrgFile(QString& _destFile, void * pHeader, void * pListItem, int iPw)
{
	QByteArray destFileName = _destFile.toLocal8Bit();
	return saveOrgHeaderAndList(destFileName.data(), pHeader, pListItem, iPw);
}

int LineDetImageProcess::createAirDat(QString & _sourceFile, QString & _installDirectory)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _installDirectory.toLocal8Bit();
	return fCreateAirRatio(sourceFileName.data(), destFileName.data());
}

int LineDetImageProcess::createBkgDat(QString & _sourceFile, QString & _installDirectory)
{
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _installDirectory.toLocal8Bit();
	return fCreateBkgData(sourceFileName.data(), destFileName.data());
}

int LineDetImageProcess::dispose(QString& _installFolder, QString& _sourceFile, QString& _destFile)
{
	QByteArray installFolder = _installFolder.toLocal8Bit();
	QByteArray sourceFileName = _sourceFile.toLocal8Bit();
	QByteArray destFileName = _destFile.toLocal8Bit();
	unsigned short setupWord = EN_BKGTUNE | EN_AIRTUNE | EN_SENSITIVE | EN_RANGE | EN_CROSSTALK 
		| EN_PULSEDOSE | EN_HARDEN | EN_ABNORMALDETECTOR;

	return fDataDispose(installFolder.data(), sourceFileName.data(), destFileName.data(), nullptr, setupWord);
}

