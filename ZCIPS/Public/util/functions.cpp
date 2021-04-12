#include "stdafx.h"
#include "functions.h"
#include <qdatetime.h>
#include <regex>
#include <qdir.h>
#include "messagebox.h"
#include <QRegExp>
#include <QValidator>

#define MB 1024*1024
QByteArray getByteArray(QString & _fileName)
{
	QByteArray byteArray = _fileName.toLocal8Bit();
	return byteArray;
}
quint64 getDiskFreeSpace(QString & _folder)
{
	LPCWSTR driver = (LPCWSTR)_folder.left(0).utf16();
	ULARGE_INTEGER freeDiskSpaceAvailable;
	GetDiskFreeSpaceEx(driver, &freeDiskSpaceAvailable, nullptr, nullptr);
	return (quint64)freeDiskSpaceAvailable.QuadPart / MB;
}

QString replaceSlashToBslash(const QString& _str)
{
	QString ret = _str;
	ret.replace('/', '\\');
	return ret;
}

QString getTimeWithUnderline()
{
	QString fileNameTime = QDateTime::currentDateTime().time().toString();
	auto i = fileNameTime.length();

	for (auto index = 0; index != i; ++index)
		if (fileNameTime[index] == ':')
			fileNameTime[index] = '-';

	return fileNameTime;
}

QString getPureFileNameFromFullName(const QString & fullName)
{
	std::regex reg("[^/]+(?=\\.)");
	std::smatch result;
	QByteArray byteArray = fullName.toLocal8Bit();
	std::string fileName = byteArray.data();
	
	if (std::regex_search(fileName, result, reg))
	{
		fileName = result.str();
		QString pureFileName = QString::fromLocal8Bit(fileName.c_str());
		return pureFileName;
	};

	return QString("");
}

QString makeFormatQString(const char* pszFmt, ...)
{
	va_list ap;
	va_start(ap, pszFmt);
	char buffer[100];
	vsprintf(buffer, pszFmt, ap);
	va_end(ap);
	return QString::fromLocal8Bit(buffer);
}

int browseAllFiles(const QString& path, std::vector<QString>& fileNames)
{
	QDir dir(path);
	int filesNumber = 0;

	if (!dir.exists())
	{
		return false;
	}

	dir.setFilter(QDir::Dirs | QDir::Files);
	dir.setSorting(QDir::DirsFirst);
	QFileInfoList list = dir.entryInfoList();
	int i = 0;

	do
	{
		QFileInfo fileInfo = list.at(i);

		if (fileInfo.fileName() == "." | fileInfo.fileName() == "..")
		{
			i++;
			continue;
		}

		bool bisDir = fileInfo.isDir();

		if (bisDir)
		{
			continue;
		}
		else
		{
			fileNames.push_back(fileInfo.fileName());
			filesNumber++;
		}

		i++;
	} while (i < list.size());
}

bool legalInputFloatOnly(const QString& _input)
{
	QRegExp exp(QString::fromLocal8Bit("^(\\d+)(\\.\\d+)?$"));

	if (exp.exactMatch(_input))
		return true;

	return false;
}

bool legalInputIntegerOnly(const QString& _input)
{
	QRegExp exp(QString::fromLocal8Bit("^[1-9]\\d*$"));

	if (exp.exactMatch(_input))
		return true;

	return false;
}

bool legalInputNoneSpecialChar(const QString& _input)
{
	QRegExp exp(QString::fromLocal8Bit("[\\\\/:*\\?<>|]"));

	if (_input.contains(exp))
		return false;

	return true;
}

void setLineEditValidaterNoSpecialChar(QLineEdit * _lineEdit)
{
	QRegExp rx = QRegExp("[^\\\\/:*?\"<>|_]*");
	QRegExpValidator* validator = new QRegExpValidator(rx);
	_lineEdit->setValidator(validator);
}
