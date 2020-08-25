
#include "ImageInstance.h"
#include "ERRInfo.h"
/**************************************************************
�������� ImageInstance

���� ��  ���캯��

��ڲ��� ��

��������ֵ ��

****************************************************************/
ImageInstance::ImageInstance()
{
	pImageProcessIns.reset(new ImageProcessInstance);
	pImageData.reset(new ImageData);
	pImageMarks.reset(new ImageMark);
	mouseType = Mouse_Default;
}

/**************************************************************
�������� LoadFromFile

���� ��  ���ļ��м���ͼ��ʵ��

��ڲ��� ��
	QString StrPath ��  �ļ�·��

��������ֵ ��
	int Err �� 0      -- ���
	           others -- δ���
****************************************************************/
int ImageInstance::LoadFromFile(QString StrInPath)
{
	// ��������
	int iRet;
	bool bRet;

	// ���ݼ���
	bRet = pImageData.get()->GetDataFromFile(StrInPath);
	if (false == bRet) return ERR_FUNC;

	return SUCCESS;
}

/**************************************************************
�������� AddProcessPipe

���� ��  ����ǰ������봦��ܵ�

��ڲ��� ��

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
int ImageInstance::AddProcessPipe()
{
	// ��������
	int iRet;
	bool bRet;
	
	// ��ǰ������
	bRet = pImageProcessIns.get()->IsCurrProcessValid();
	if (true != bRet) return ERR_NO_CURR_PROCESS;
	
	// ����ͼ����ܵ�
	iRet = pImageProcessIns.get()->AddCurrProcessToPipe();
	if (SUCCESS != iRet) return ERR_FUNC;

	return SUCCESS;
}

/**************************************************************
�������� DataProccessAccept

���� ��  ��ǰͼ�����ý��ܣ�
		 ���ݷ����ı�
		 ��pfCurrData -> pfProcessingData

��ڲ��� ��

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
int ImageInstance::DataProccessAccept()
{
	// ��������
	int iRet;
	
	// pfCurrData -> pfProcessingData
	iRet = pImageData.get()->ProcessingAccept();
	if (SUCCESS != iRet) return ERR_FUNC;

	return SUCCESS;
}

/**************************************************************
�������� IsCurrProcessValid

���� ��  ��ǰͼ�����Ƿ����

��ڲ��� ��

��������ֵ ��
	true  �� ����
	false �� ������
****************************************************************/
bool ImageInstance::IsCurrProcessValid()
{
	return pImageProcessIns.get()->IsCurrProcessValid();
}

/**************************************************************
�������� IsCurrProcessValid

���� ��  ��ǰͼ����ܵ��Ƿ����

��ڲ��� ��

��������ֵ ��
	true  �� ����
	false �� ������
****************************************************************/
bool ImageInstance::IsProcessPipeValid()
{
	// �ж�ͼ����ʵ���Ƿ����
	if (true == pImageProcessIns.isNull()) return false;
	
	// �ж�ͼ����ܵ�
	if (false == pImageProcessIns.get()->IsProcessPipeValid()) return false;

	return true;
}

/**************************************************************
�������� Copy

���� ��  ����

��ڲ��� ��
	QSharedPointer<ImageInstance>& pImageIns �� ͼ��ʵ��

��������ֵ ��
	true  �� �ɹ�
	false �� ʧ��
****************************************************************/
bool ImageInstance::Copy(QSharedPointer<ImageInstance>& pImageIns)
{
	// ��ʼ��
	pImageIns.reset(new ImageInstance);

	// �ڲ�Ԫ�ؿ���
	pImageProcessIns.get()->Copy(pImageIns.get()->pImageProcessIns);
	pImageData.get()->Copy(pImageIns.get()->pImageData);

	return true;
}

/**************************************************************
�������� LoadImageProPipe

���� ��  �����µ�ͼ����ܵ�
		 ���Ե�ǰͼ������ִ�и�ͼ����ܵ�

��ڲ��� ��
	QSharedPointer<ImageProPipe> pInProcessPipe ��

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
int ImageInstance::LoadImageProPipe
(
	QSharedPointer<ImageProPipe> pInProcessPipe
)
{
	// ��������
	int iRet;
	
	// ��μ��
	if (true == pInProcessPipe.isNull()) return ERR_INPUT;

	// ����ͼ����ܵ�
	iRet = pImageProcessIns.get()->LoadImageProPipe(pInProcessPipe);
	if (SUCCESS != iRet) return ERR_FUNC;

	// ͼ��ܵ�ִ��
	iRet = ProPipeExcute();
	if (SUCCESS != iRet) return ERR_FUNC;

	return SUCCESS;
}

/**************************************************************
�������� ProPipeExcute

���� ��  ִ��ͼ����ܵ�

��ڲ��� ��

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
int ImageInstance::ProPipeExcute()
{
	// ��������
	int iRet;

	// ָ����
	if ((true == pImageData.isNull()) || (true == pImageProcessIns.isNull()))
		return ERR_INPUT;

	// ��������
	iRet = pImageData.get()->ResetData();
	if (SUCCESS != iRet) return ERR_FUNC;

	// ִ��ͼ����ܵ�
	iRet = pImageProcessIns.get()->ProPipeExcute(
		pImageData.get()->pfOrgData,
		pImageData.get()->pfProcessingData,
		pImageData.get()->iWidth,
		pImageData.get()->iHeight);
	if (SUCCESS != iRet) return ERR_FUNC;

	// 
	iRet = pImageData.get()->ProcessingAccept();
	if (SUCCESS != iRet) return ERR_FUNC;

	return SUCCESS;
}

/**************************************************************
�������� ProPipeCut

���� ��  ����ܵ���ȡ

��ڲ��� ��
	int iCutProNo �� ��ȡ������

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
int ImageInstance::ProPipeCut(int iCutProNo)
{
	// ��������
	int iRet;      // ��������ֵ

	// ����ܵ���ȡ
	iRet = pImageProcessIns.get()->ProPipeCut(iCutProNo);
	if (SUCCESS != iRet) return ERR_FUNC;

	return SUCCESS;
}


/**************************************************************
�������� ProPipeSize

���� ��  ����ܵ����Ȼ�ȡ

��ڲ��� ��

��������ֵ ��
	int iLen �� ����ܵ�����
****************************************************************/
int ImageInstance::ProPipeSize()
{
	return pImageProcessIns.get()->pProcessPipe.get()->GetPipeLen();
}

/**************************************************************
�������� ClearCurrProcess

���� ��  �����ǰ����

��ڲ��� ��

��������ֵ ��
	
****************************************************************/
void ImageInstance::ClearCurrProcess()
{
	pImageProcessIns.get()->ClearCurrProcess();
}

/**************************************************************
�������� ClearProPipe

���� ��  �������ܵ�

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ImageInstance::ClearProPipe()
{
	pImageProcessIns.get()->ClearProPipe();
}

/**************************************************************
�������� ResetImgData

���� ��  ��������

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ImageInstance::ResetImgData()
{
	pImageData.get()->ResetData();
}







/**************************************************************
�������� Size

���� ��  ��ȡMap����

��ڲ��� ��

��������ֵ ��
	int size ��Map����ֵ
****************************************************************/
int ImageInstanceMap::Size()
{
	return (int)SortList.size();
}

/**************************************************************
�������� Insert

���� ��  ��ͼ��ʵ������ͼ��Map

��ڲ��� ��

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
void ImageInstanceMap::Insert
(
	QString StrLabel,
	QSharedPointer<ImageInstance> pImageIns
)
{
	// ��������
	QMap<QString, QSharedPointer<ImageInstance>>::const_iterator it;    // QMap�ĵ�����

	// ����Ƿ����ظ���ͼ��ʵ��
	it = MapImageIns.find(StrLabel);
	if (MapImageIns.end() != it) return;

	// ����ͼ��ʵ��
	MapImageIns.insert(StrLabel, pImageIns);
	SortList.append(StrLabel);
}

/**************************************************************
�������� KeyAt

���� ��  ���������Ż�ȡKeyֵ

��ڲ��� ��
	int iNo ��        ���

��������ֵ ��
	QString StrKey �� Keyֵ
****************************************************************/
QString ImageInstanceMap::KeyAt(int iNo)
{
	// ��������
	QString StrKey;

	// ��ʼ��
	StrKey.clear();

	// ����ж�
	if ((iNo < 0) || (iNo >= SortList.size())) return StrKey;

	// Keyֵ��ֵ
	StrKey = SortList[iNo];

	return StrKey;
}

/**************************************************************
�������� ValueAt

���� ��  ���������Ż�ȡValueֵ

��ڲ��� ��
	int iNo ��        ���

��������ֵ ��
	QSharedPointer<ImageInstance>  �� Valueֵ
****************************************************************/
QSharedPointer<ImageInstance> ImageInstanceMap::ValueAt(int iNo)
{
	// ��������
	QString StrKey;
	QSharedPointer<ImageInstance> pImageIns;
	QMap<QString, QSharedPointer<ImageInstance>>::iterator it;
	
	// ��ʼ��
	pImageIns.clear();

	// ����ж�
	if ((iNo < 0) || (iNo >= SortList.size())) return pImageIns;

	// keyֵ��ȡ
	StrKey = SortList[iNo];

	// ����ָ���ȡ
	it = MapImageIns.find(StrKey);

	return it.value();
}

/**************************************************************
�������� Find

���� ��  ����keyֵѰ��valueֵ

��ڲ��� ��
	int iNo ��        ���

��������ֵ ��
	QSharedPointer<ImageInstance>  �� Valueֵ
****************************************************************/
QSharedPointer<ImageInstance> ImageInstanceMap::Find(QString StrKey)
{
	// ��������
	QSharedPointer<ImageInstance> pImageIns;
	QMap<QString, QSharedPointer<ImageInstance>>::iterator it;

	// ��ʼ��
	pImageIns.clear();

	// ����ָ���ȡ
	it = MapImageIns.find(StrKey);
	if (it == MapImageIns.cend()) return pImageIns;
	
	return it.value();
}
/***********************************************************************/
	//��ͼ��ʵ����Map��ɾ��
void ImageInstanceMap::Delete(QString StrLabel)
{
	QMap<QString, QSharedPointer<ImageInstance>>::iterator it;
	for (it = MapImageIns.begin(); it != MapImageIns.cend(); )
	{
		if (it.key() == StrLabel)
		{
			MapImageIns.erase(it);
			for (int i = 0; i < SortList.size(); i++)
			{
				if (SortList.at(i) == StrLabel)
				{
					SortList.removeAt(i);
					break;
				}
			}
			break;
		}
		else
		{
			it++;
		}
	}
}