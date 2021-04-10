#pragma once

#include <QObject>
#include <windows.h>
QByteArray getByteArray(QString& _fileName);
quint64 getDiskFreeSpace(QString & _folder);
QString replaceSlashToBslash(const QString& _str);
QString getTimeWithUnderline();
int browseAllFiles(const QString& path, std::vector<QString>& fileNames);
QString getPureFileNameFromFullName(const QString& fullName);
QString makeFormatQString(const char* pszFmt, ...);