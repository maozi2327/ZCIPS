#include "stdafx.h"
#include "logmacro.h"

QString makeMessage(const char* pszFmt, ...)
{	
	va_list ap;
	va_start(ap, pszFmt);
	char buffer[100];
	vsprintf(buffer, pszFmt, ap);
	va_end(ap);
	return QString::fromLocal8Bit(buffer);
}

QString makeMessage(const QString & pszFmt)
{
	return pszFmt;
}
