#pragma once

#include <QtWidgets>

// ����ѡ����
class AreaSelect
{
public:
	int iHeight;
	int iWidth;
	QSharedPointer<UCHAR> pucAreaMask;                          // ����ѡ������

public:

	void InitMask(QRect RectMask);

	bool Copy(QSharedPointer<AreaSelect>& pCpAreaSelect);
};
