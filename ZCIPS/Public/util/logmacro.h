#pragma once
#include "stdafx.h"

#include <QString>
#define LOGSIGNAL	void infoMsgSignal(QString info);\
					void errorMsgSignal(QString error);\
					void bugMsgSignal(QString bug);

QString makeFormatQString(const char* pszFmt, ...);
QString makeFormatQString(const QString& pszFmt);

#define LOG_INFO(fmt, ...) emit(infoMsgSignal(QString("[") + makeFormatQString(fmt,##__VA_ARGS__) + ']'))

#define LOG_ERROR(fmt, ...) emit(errorMsgSignal(QString("[") + makeFormatQString(fmt,##__VA_ARGS__) + ']'))

#define LOG_BUG(fmt, ...) emit(bugMsgSignal(QString("[") + makeFormatQString(fmt,##__VA_ARGS__) + ']' + \
	'[' + QString(__FUNCSIG__) + ']' + '[' + QString::number(__LINE__) + ']'))

