#pragma once
#include "../Public/lib/CtDispose.h"

class LineDetImageProcess
{
public:
	LineDetImageProcess();
	~LineDetImageProcess();

	int saveOrgFile(QString& _destFile, void *pHeader, void *pListItem, int iPw);
	int createAirDat(QString& _sourceFile, QString& _installDirectory);
	int createBkgDat(QString& _sourceFile, QString& _installDirectory);
	int dispose(QString& _installFolder, QString& _sourceFile, QString& _destFile);
};

