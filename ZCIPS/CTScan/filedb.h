#pragma once
#include <QObject>

struct CT3Record
{
	QString path;
	QString fileName;
	int number;
	int view;
	int matrix;
	int sampleTime;
	float layer;
	QString remark;
};

struct OrgRecord
{
	QString path;
	QString fileName;
	int number;
	char type;
	QString remark;
};

class FileDB : public QObject
{
	Q_OBJECT

public:
	FileDB(QObject *parent);
	~FileDB();

	QString getNumber(const QString& _fileName);
	bool writeCt3Record(const CT3Record& _record);
	bool writeOrgRecord(const OrgRecord& _record);
};
