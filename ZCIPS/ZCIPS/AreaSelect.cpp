#include "AreaSelect.h"

/**************************************************************
函数名： ZCIPSMainFrame

定义 ：  构造函数

入口参数 ：
	QRect RectMask :  掩码区域

函数返回值 ：

****************************************************************/
void AreaSelect::InitMask(QRect RectMask)
{
	pucAreaMask.reset(new UCHAR[RectMask.height() * RectMask.width()]);
	memset(pucAreaMask.get(), (UCHAR)1, RectMask.height() * RectMask.width());

	iHeight = RectMask.height();
	iWidth = RectMask.width();
}

/**************************************************************
函数名： Copy

定义 ：  拷贝函数

入口参数 ：
	QSharedPointer<AreaSelect>& pCpAreaSelect :  拷贝指针

函数返回值 ：
	true  ： 成功
	false ： 失败
****************************************************************/
bool AreaSelect::Copy(QSharedPointer<AreaSelect>& pCpAreaSelect)
{
	// 参数声明
	int iHeight;
	int iWidth;
	
	// 初始化
	pCpAreaSelect.reset(new AreaSelect);
	
	// 参数获取
	iHeight = this->iHeight;
	iWidth = this->iWidth;

	// 赋值
	pCpAreaSelect.get()->iHeight = iHeight;
	pCpAreaSelect.get()->iWidth = iWidth;
	pCpAreaSelect.get()->pucAreaMask.reset(new UCHAR[iHeight * iWidth]);
	memcpy_s(pCpAreaSelect.get()->pucAreaMask.get(),
		sizeof(UCHAR) * iHeight * iWidth,
		this->pucAreaMask.get(),
		sizeof(UCHAR) * iHeight * iWidth);

	return true;
}