#include "stdafx.h"
#include "filedb.h"

FileDB::FileDB(QObject *parent)
	: QObject(parent)
{
}

FileDB::~FileDB()
{
}

QString FileDB::getNumber(const QString& _fileName)
{
	return QString();
}

bool FileDB::writeCt3Record(const CT3Record & _record)
{
	return false;
}

bool FileDB::writeOrgRecord(const OrgRecord & _record)
{
	return false;
}
