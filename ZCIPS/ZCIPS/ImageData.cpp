#include "ImageData.h"
#include "FreeImage.h"
#include "ERRInfo.h"

// ���lib�ļ�
#pragma comment(lib, "FreeImage.lib")

/**************************************************************
�������� ZCIPSData

���� ��  ���캯��

��ڲ��� ��

��������ֵ ��

****************************************************************/
ImageData::ImageData()
{
	pfOrgData.clear();
	pfCurrData.clear();
	pfProcessingData.clear();
	iHeight = 0;
	iWidth = 0;
}

/**************************************************************
�������� InitData

���� ��  ��ʼ������

��ڲ��� ��
	int iInHeight �� ͼ��߶�
	int iInWidth ��  ͼ����

��������ֵ ��
	true ��		�ɹ�
	false ��	ʧ��
****************************************************************/
bool ImageData::InitData
(
	int iInHeight,
	int iInWidth
)
{
	// ��μ��
	if ((0 > iInHeight) || (0 > iInWidth))
	{
		return false;
	}

	// ��ʼ��
	iHeight = iInHeight;
	iWidth = iInWidth;
	pfOrgData.reset(new float[iInHeight * iInWidth]);
	pfCurrData.reset(new float[iInHeight * iInWidth]);
	pfProcessingData.reset(new float[iInHeight * iInWidth]);

	return true;
}


/**************************************************************
�������� ClearData

���� ��  �������

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ImageData::ClearData()
{
	pfOrgData.clear();
	pfCurrData.clear();
	pfProcessingData.clear();
	iHeight = 0;
	iWidth = 0;
}

/**************************************************************
�������� GetDataFromFile

���� ��  ���ļ��м�������

��ڲ��� ��
	QString StrFileName ��	

��������ֵ ��
	true ��		�ɹ�
	false ��	ʧ��
****************************************************************/
bool ImageData::GetDataFromFile(QString StrFileName)
{
	// ��������
	bool bRet;                     // ��������ֵ
	int iHeight;                   // ͼ��߶�
	int iWidth;                    // ͼ����
	int iPixBits;                  // ���ؽṹ��Ԫ
	FREE_IMAGE_FORMAT FIFormat;    // ͼƬ����
	FIBITMAP *FIBmp;               // ���ص�ͼ��
	unsigned char *pucDataPt;
	unsigned short *pusDataPt;

	// ��ʼ��
	FIBmp = nullptr;

	// ��ȡͼƬ����
	FIFormat = FreeImage_GetFileTypeU
	(reinterpret_cast<const wchar_t *>(StrFileName.utf16()));
	
	// ����ͼ�񲢻�ȡ����
	FIBmp = FreeImage_LoadU(FIFormat, reinterpret_cast<const wchar_t *>(StrFileName.utf16()));
	if (nullptr == FIBmp) return false;
	iPixBits = FreeImage_GetBPP(FIBmp);
	iHeight = FreeImage_GetHeight(FIBmp);
	iWidth = FreeImage_GetWidth(FIBmp);

	// ��ʼ��
	bRet = InitData(iHeight, iWidth);
	if (false == bRet) return false;

	// ��������
	switch (iPixBits)
	{
		// 8λ����
		case 8:
		{
			// ��ȡ����ָ��
			pucDataPt = FreeImage_GetBits(FIBmp);

			// ���ݸ�ֵ
			for (int Loop_H = 0; Loop_H < iHeight; Loop_H++)
				for (int Loop_W = 0; Loop_W < iWidth; Loop_W++)
				{
					pfOrgData.get()[Loop_H * iWidth + Loop_W]
						= (float)pucDataPt[(iHeight - Loop_H - 1) * iWidth + Loop_W];
					pfCurrData.get()[Loop_H * iWidth + Loop_W]
						= pfOrgData.get()[Loop_H * iWidth + Loop_W];
					pfProcessingData.get()[Loop_H * iWidth + Loop_W]
						= pfOrgData.get()[Loop_H * iWidth + Loop_W];
				}

			break;
		}
		// 16λ����
		case 16:
		{
			// ��ȡ����ָ��
			pusDataPt = (unsigned short*)FreeImage_GetBits(FIBmp);

			// ���ݸ�ֵ
			for (int Loop_H = 0; Loop_H < iHeight; Loop_H++)
				for (int Loop_W = 0; Loop_W < iWidth; Loop_W++)
				{
					pfOrgData.get()[Loop_H * iWidth + Loop_W]
						= (float)pusDataPt[(iHeight - Loop_H - 1) * iWidth + Loop_W] / (float)65535 * (float)255;
					pfCurrData.get()[Loop_H * iWidth + Loop_W]
						= pfOrgData.get()[Loop_H * iWidth + Loop_W];
					pfProcessingData.get()[Loop_H * iWidth + Loop_W]
						= pfOrgData.get()[Loop_H * iWidth + Loop_W];
				}

			break;
		}
		// 24λ����
		case 24:
		{
			// ��ȡ����ָ��
			pucDataPt = FreeImage_GetBits(FIBmp);

			// RGB��ɫת�Ҷ�ֵ
			for (int Loop_H = 0; Loop_H < iHeight; Loop_H++)
				for (int Loop_W = 0; Loop_W < iWidth; Loop_W++)
				{
					pfOrgData.get()[Loop_H * iWidth + Loop_W]
						= (float)pucDataPt[(iHeight - Loop_H - 1) * iWidth * 3 + Loop_W] * 0.3		 // Red
						+ (float)pucDataPt[(iHeight - Loop_H - 1) * iWidth * 3 + Loop_W + 1] * 0.59  // Green
						+ (float)pucDataPt[(iHeight - Loop_H - 1) * iWidth * 3 + Loop_W + 2] * 0.11; // blue
					pfCurrData.get()[Loop_H * iWidth + Loop_W]
						= pfOrgData.get()[Loop_H * iWidth + Loop_W];
					pfProcessingData.get()[Loop_H * iWidth + Loop_W]
						= pfOrgData.get()[Loop_H * iWidth + Loop_W];
				}

			break;
		}
		// 32λ����
		case 32:
		{
			// ��ȡ����ָ��
			pucDataPt = FreeImage_GetBits(FIBmp);

			// RGB��ɫת�Ҷ�ֵ
			for (int Loop_H = 0; Loop_H < iHeight; Loop_H++)
				for (int Loop_W = 0; Loop_W < iWidth; Loop_W++)
				{
					pfOrgData.get()[Loop_H * iWidth + Loop_W]
						= (float)pucDataPt[(iHeight - Loop_H - 1) * iWidth * 4 + Loop_W] * 0.3		 // Red
						+ (float)pucDataPt[(iHeight - Loop_H - 1) * iWidth * 4 + Loop_W + 1] * 0.59  // Green
						+ (float)pucDataPt[(iHeight - Loop_H - 1) * iWidth * 4 + Loop_W + 2] * 0.11; // blue
					pfCurrData.get()[Loop_H * iWidth + Loop_W]
						= pfOrgData.get()[Loop_H * iWidth + Loop_W];
					pfProcessingData.get()[Loop_H * iWidth + Loop_W]
						= pfOrgData.get()[Loop_H * iWidth + Loop_W];
				}

			break;
		}
		// 
		default:
		{
			return false;
		}
	}
	
	return true;
}

/**************************************************************
�������� ProcessingAccept

���� ��  ���ڴ����ͼ���ý���

��ڲ��� ��

��������ֵ ��
	int iRet ��
****************************************************************/
int ImageData::ProcessingAccept()
{
	// ���������ݼ��
	if (true == pfProcessingData.isNull()) return ERR_INPUT;
	
	// ��ʼ��
	pfCurrData.reset(new float[iHeight * iWidth]);

	// �������е�ͼ�����ݸ�ֵ����ǰ��������
	memcpy_s(pfCurrData.get(), iHeight * iWidth * sizeof(float),
		pfProcessingData.get(), iHeight * iWidth * sizeof(float));

	return SUCCESS;
}

/**************************************************************
�������� ProcessingCancel

���� ��  ���ڴ����ͼ�񱻳���

��ڲ��� ��

��������ֵ ��
	int iRet ��
****************************************************************/
int ImageData::ProcessingCancel()
{
	// ���������ݼ��
	if (true == pfCurrData.isNull()) return ERR_INPUT;

	// ��ʼ��
	pfProcessingData.reset(new float[iHeight * iWidth]);

	// �������е�ͼ�����ݸ�ֵ����ǰ��������
	memcpy_s(pfProcessingData.get(), iHeight * iWidth * sizeof(float),
		pfCurrData.get(), iHeight * iWidth * sizeof(float));

	return SUCCESS;
}

/**************************************************************
�������� ResetData

���� ��  ��������
         ������Ϊԭʼ����

��ڲ��� ��

��������ֵ ��
	int iRet ��
****************************************************************/
int ImageData::ResetData()
{
	// Դ���ݼ��
	if (true == pfOrgData.isNull()) return ERR_INPUT;

	// ��ʼ��
	pfProcessingData.reset(new float[iHeight * iWidth]);
	pfCurrData.reset(new float[iHeight * iWidth]);

	// ���ݸ�ֵ
	memcpy_s(pfCurrData.get(), iHeight * iWidth * sizeof(float),
		pfOrgData.get(), iHeight * iWidth * sizeof(float));
	memcpy_s(pfProcessingData.get(), iHeight * iWidth * sizeof(float),
		pfOrgData.get(), iHeight * iWidth * sizeof(float));

	return SUCCESS;
}

/**************************************************************
�������� Copy

���� ��  ��������

��ڲ��� ��
	QSharedPointer<ImageData> pCpImageData �� ����ָ��

��������ֵ ��
	true  �� �ɹ�
	false �� ʧ��
****************************************************************/
bool ImageData::Copy(QSharedPointer<ImageData>& pCpImageData)
{
	// ��������
	int iHeight;
	int iWidth;
	
	// ��ʼ��
	pCpImageData.reset(new ImageData);

	// ��ֵ
	iHeight = this->iHeight;
	iWidth = this->iWidth;

	// ��ֵ
	pCpImageData.get()->iHeight = iHeight;
	pCpImageData.get()->iWidth = iWidth;
	pCpImageData.get()->pfOrgData.reset(new float[iHeight * iWidth]);
	pCpImageData.get()->pfCurrData.reset(new float[iHeight * iWidth]);
	pCpImageData.get()->pfProcessingData.reset(new float[iHeight * iWidth]);
	memcpy_s(pCpImageData.get()->pfOrgData.get(), iHeight * iWidth * sizeof(float),
		this->pfOrgData.get(), iHeight * iWidth * sizeof(float));
	memcpy_s(pCpImageData.get()->pfCurrData.get(), iHeight * iWidth * sizeof(float),
		this->pfCurrData.get(), iHeight * iWidth * sizeof(float));
	memcpy_s(pCpImageData.get()->pfProcessingData.get(), iHeight * iWidth * sizeof(float),
		this->pfProcessingData.get(), iHeight * iWidth * sizeof(float));

	return true;
}