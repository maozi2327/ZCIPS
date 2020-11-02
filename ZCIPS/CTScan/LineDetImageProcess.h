#pragma once
#include "../Public/lib/CtDispose.h"
#include <QString>

class LineDetImageProcess
{
protected:
	QString d_orgPath;
	QString d_ctPath;
	QString d_drPath;
	QString d_workDirectory;
public:
	LineDetImageProcess(const QString& _workDirectory, const QString& _orgPath
		, const QString& _ctPath, const QString& _drPath);
	~LineDetImageProcess();


	int saveOrgFile(const QString& _destFile, void *pHeader, void *pListItem, int iPw);
	int createAirDat(const QString& _sourceFile);
	int createBkgDat(const QString& _sourceFile);
	int dispose(const QString& _sourceFile);
	int ct3Dispose(const QString& _sourceFile);
	int drDispose(const QString& _sourceFile);
};

