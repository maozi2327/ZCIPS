#include "AreaSelect.h"

/**************************************************************
�������� ZCIPSMainFrame

���� ��  ���캯��

��ڲ��� ��
	QRect RectMask :  ��������

��������ֵ ��

****************************************************************/
void AreaSelect::InitMask(QRect RectMask)
{
	pucAreaMask.reset(new UCHAR[RectMask.height() * RectMask.width()]);
	memset(pucAreaMask.get(), (UCHAR)1, RectMask.height() * RectMask.width());

	iHeight = RectMask.height();
	iWidth = RectMask.width();
}

/**************************************************************
�������� Copy

���� ��  ��������

��ڲ��� ��
	QSharedPointer<AreaSelect>& pCpAreaSelect :  ����ָ��

��������ֵ ��
	true  �� �ɹ�
	false �� ʧ��
****************************************************************/
bool AreaSelect::Copy(QSharedPointer<AreaSelect>& pCpAreaSelect)
{
	// ��������
	int iHeight;
	int iWidth;
	
	// ��ʼ��
	pCpAreaSelect.reset(new AreaSelect);
	
	// ������ȡ
	iHeight = this->iHeight;
	iWidth = this->iWidth;

	// ��ֵ
	pCpAreaSelect.get()->iHeight = iHeight;
	pCpAreaSelect.get()->iWidth = iWidth;
	pCpAreaSelect.get()->pucAreaMask.reset(new UCHAR[iHeight * iWidth]);
	memcpy_s(pCpAreaSelect.get()->pucAreaMask.get(),
		sizeof(UCHAR) * iHeight * iWidth,
		this->pucAreaMask.get(),
		sizeof(UCHAR) * iHeight * iWidth);

	return true;
}