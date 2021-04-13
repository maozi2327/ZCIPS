#pragma once

#include <QObject>
#include <windows.h>
#include <qlineedit.h>
#include <vector>

QByteArray getByteArray(QString& _fileName);
quint64 getDiskFreeSpace(QString & _folder);
QString replaceSlashToBslash(const QString& _str);
QString getTimeWithUnderline();
int browseAllFiles(const QString& path, std::vector<QString>& fileNames);
QString getPureFileNameFromFullName(const QString& fullName);
QString makeFormatQString(const char* pszFmt, ...);
bool legalInputFloatOnly(const QString& _input);
bool legalInputIntegerOnly(const QString& _input);
bool legalInputNoneSpecialChar(const QString& _input);

void setLineEditValidaterNoSpecialChar(QLineEdit* _lineEdit);
void setLineEditValidaterFloatChar(QLineEdit* _lineEdit);
void setLineEditValidaterIntegerChar(QLineEdit* _lineEdit);

void setLineEditValidaterNoSpecialChar(std::vector<QLineEdit*>& _lineEdit);
void setLineEditValidaterFloatChar(std::vector<QLineEdit*>& _lineEdit);
void setLineEditValidaterIntegerChar(std::vector<QLineEdit*>& _lineEdit);
