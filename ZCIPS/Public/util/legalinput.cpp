#include "stdafx.h"
#include "legalinput.h"
#include "messagebox.h"
#include <QRegExp>

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


