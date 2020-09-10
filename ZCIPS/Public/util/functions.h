#pragma once

#include <QObject>
#include <windows.h>
QByteArray getByteArray(QString& _fileName);
quint64 getDiskFreeSpace(QString & _folder);
