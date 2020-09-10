#include "stdafx.h"
#include "functions.h"
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
