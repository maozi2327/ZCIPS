#pragma once
#include "stdafx.h"

#include <QString>
#define LOGSIGNAL	void infoMsgSignal(QString info);\
					void errorMsgSignal(QString error);\
					void bugMsgSignal(QString bug);

QString makeMessage(const char* pszFmt, ...);
QString makeMessage(const QString& pszFmt);

#define LOG_INFO(fmt, ...) emit(infoMsgSignal(QString("[") + makeMessage(fmt,##__VA_ARGS__) + ']'))

#define LOG_ERROR(fmt, ...) emit(errorMsgSignal(QString("[") + makeMessage(fmt,##__VA_ARGS__) + ']'))

#define LOG_BUG(fmt, ...) emit(bugMsgSignal(QString("[") + makeMessage(fmt,##__VA_ARGS__) + ']' + \
	'[' + QString(__FUNCSIG__) + ']' + '[' + QString::number(__LINE__) + ']'))

