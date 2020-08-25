#pragma once

#include <QtWidgets>

// 区域选择类
class AreaSelect
{
public:
	int iHeight;
	int iWidth;
	QSharedPointer<UCHAR> pucAreaMask;                          // 区域选择掩码

public:

	void InitMask(QRect RectMask);

	bool Copy(QSharedPointer<AreaSelect>& pCpAreaSelect);
};
