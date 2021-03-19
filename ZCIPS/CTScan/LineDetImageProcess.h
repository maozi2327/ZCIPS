#pragma once
#include "../Public/lib/CtDispose.h"
#include <QString>

class LineDetImageProcess
{
protected:
	QString d_workDirectory;
public:
	LineDetImageProcess(const QString& _workDirectory);
	~LineDetImageProcess();


	int saveOrgFile(const QString& _destFile, void *pHeader, void *pListItem, int iPw);
	int createAirDat(const QString& _sourceFile, const QString& _destDirectory);
	int createBkgDat(const QString& _sourceFile, const QString& _destPath);
	int ct3Dispose(const QString& _orgFile, const QString& _destPath);
	int drDispose(const QString& _sourceFile);
};

