#include <opencv2/opencv.hpp>
#include <math.h>
#include "ImageProcess.h"
#include "ERRInfo.h"


// ���lib�ļ�
#pragma comment(lib, OPENCV_LIB)

using namespace std;
using namespace cv;

/*******************************  Start:GrayTransProcess   ************************************/

/**************************************************************
�������� GrayTransProcess

���� ��  ���캯��

��ڲ��� ��

��������ֵ ��

****************************************************************/
GrayTransProcess::GrayTransProcess()
{
	fBottom = (float)0;
	fTop = (float)255;
	fR = (float)1;
	pAreaSelect.clear();
}

/**************************************************************
�������� ImagePro

���� ��  ͼ����

��ڲ��� ��
	QSharedPointer<float> pfSrc �� 
	QSharedPointer<float> pfDes ��
	int iWidth �� 
	int iHeight ��

��������ֵ ��
	int iRet ��
****************************************************************/
int GrayTransProcess::ImagePro
(
	QSharedPointer<float> pfSrc,
	QSharedPointer<float> pfDes,
	int iWidth, 
	int iHeight
)
{
	// ��������
	Mat MatProcess;

	// ��μ��
	if ((true == pfSrc.isNull()) || (true == pfDes.isNull()))
		return ERR_INPUT;

	// ���ݾ����ʼ��
	MatProcess = Mat(iHeight, iWidth, CV_32F);
	for (int i = 0; i < iHeight; i++)
		for (int j = 0; j < iWidth; j++)
			MatProcess.at<float>(i, j) = pfSrc.get()[i * iWidth + j];
	
	//// ���Դ���
	//Mat MatTest = Mat(iHeight, iWidth, CV_8U);
	//for (int i = 0; i < iHeight; i++)
	//	for (int j = 0; j < iWidth; j++)
	//	{
	//		MatTest.at<UCHAR>(i, j) = (UCHAR)MatProcess.at<float>(i, j);
	//	}
	//imwrite("D:\\a.png", MatTest);

	// ���лҶ����Ա任
	for (int i = 0; i < iHeight; i++)
		for (int j = 0; j < iWidth; j++)
		{
			if (MatProcess.at<float>(i, j) < fBottom)
				MatProcess.at<float>(i, j) = fBottom;
			else if (MatProcess.at<float>(i, j) > fTop)
				MatProcess.at<float>(i, j) = fTop;
		}
	MatProcess = MatProcess - fBottom;
	MatProcess.convertTo(MatProcess, CV_32F, (double)(255 / (fTop - fBottom)), (double)0);
	
	// ���лҶ�ָ���任
	pow(MatProcess / 255, (double)fR, MatProcess);
	MatProcess = MatProcess * 255;

	// �������
	if (false == pAreaSelect.isNull())  // ����ѡ��
	{
		for (int i = 0; i < iHeight; i++)
			for (int j = 0; j < iWidth; j++)
			{
				if ((UCHAR)1 == pAreaSelect.get()->pucAreaMask.get()[i * iWidth + j])
				{
					pfDes.get()[i * iWidth + j] = MatProcess.ptr(0)[i * iWidth + j];
				}
				else
				{
					pfDes.get()[i * iWidth + j] = pfSrc.get()[i * iWidth + j];
				}
			}
	}
	else	// ������ѡ��
	{
		memcpy_s(pfDes.get(), iHeight * iWidth * sizeof(float),
			MatProcess.ptr(0), iHeight * iWidth * sizeof(float));
	}

	return SUCCESS;
}

/**************************************************************
�������� SetPara

���� ��  ��������

��ڲ��� ��
	float fInBottom,
	float fInTop, 
	float fR

��������ֵ ��

****************************************************************/
void GrayTransProcess::SetPara(float fInBottom, float fInTop, float fInR)
{
	SetBottom(fInBottom);
	SetTop(fInTop);
	SetR(fInR);
}

/**************************************************************
�������� SetBottom

���� ��  ��������

��ڲ��� ��
	float fInBottom

��������ֵ ��

****************************************************************/
void GrayTransProcess::SetBottom(float fInBottom)
{
	fBottom = fInBottom;
}

/**************************************************************
�������� SetTop

���� ��  ��������

��ڲ��� ��
	float fInTop

��������ֵ ��

****************************************************************/
void GrayTransProcess::SetTop(float fInTop)
{
	fTop = fInTop;
}

/**************************************************************
�������� SetR

���� ��  R����

��ڲ��� ��
	float fInR

��������ֵ ��

****************************************************************/
void GrayTransProcess::SetR(float fInR)
{
	fR = fInR;
}

/**************************************************************
�������� GetBottom

���� ��  ��ȡBottomֵ

��ڲ��� ��
	
��������ֵ ��
	float fBottom ������ֵ
****************************************************************/
float GrayTransProcess::GetBottom() { return fBottom; }

/**************************************************************
�������� GetBottom

���� ��  ��ȡBottomֵ

��ڲ��� ��

��������ֵ ��
	float fR ��Rֵ
****************************************************************/
float GrayTransProcess::GetR() { return fR; }

/**************************************************************
�������� GetBottom

���� ��  ��ȡBottomֵ

��ڲ��� ��

��������ֵ ��
	float fTop ������ֵ
****************************************************************/
float GrayTransProcess::GetTop() { return fTop; }

/**************************************************************
�������� setArea

���� ��  ������������

��ڲ��� ��
	QSharedPointer<AreaSelect> pInAreaSel

��������ֵ ��

****************************************************************/
void GrayTransProcess::setArea(QSharedPointer<AreaSelect> pInAreaSel)
{
	pAreaSelect.reset(new AreaSelect());

	pAreaSelect.get()->iHeight = pInAreaSel.get()->iHeight;
	pAreaSelect.get()->iWidth = pInAreaSel.get()->iWidth;
	pAreaSelect.get()->pucAreaMask.reset(
		new UCHAR[pAreaSelect.get()->iHeight * pAreaSelect.get()->iWidth]);
	memcpy_s(pAreaSelect.get()->pucAreaMask.get(), pAreaSelect.get()->iHeight * pAreaSelect.get()->iWidth,
		pInAreaSel->pucAreaMask.get(), pAreaSelect.get()->iHeight * pAreaSelect.get()->iWidth);

}

/**************************************************************
�������� GetParaString

���� ��  ��ȡ�����ַ�

��ڲ��� ��
	QString &StrPara

��������ֵ ��
	
****************************************************************/
int GrayTransProcess::GetParaString(QString &StrPara)
{
	// ��������
	QTextCodec* pTCode;							// �ַ����ͱ任ָ��

	// ��ʼ��
	StrPara.clear();

	// ��ȡ�ļ��Ի���
	pTCode = QTextCodec::codecForName("GBK");
	StrPara = pTCode->toUnicode("����ֵ: ") + QString::number((double)fBottom, 'g', 3) + '\n'
		+ pTCode->toUnicode("����ֵ: ") + QString::number((double)fTop, 'g', 3) + '\n'
		+ pTCode->toUnicode("ָ��ֵ: ") + QString::number((double)fR, 'g', 3);
	if (false == pAreaSelect.isNull())
	{
		StrPara += '\n' + pTCode->toUnicode("�����ȡ");
	}
		
	return SUCCESS;
}

/**************************************************************
�������� Copy

���� ��  ��������

��ڲ��� ��
	GrayTransProcess** pCpProcess ��������ָ��

��������ֵ ��
	true  �� ��ȷ
	false �� ����
****************************************************************/
bool GrayTransProcess::Copy(GrayTransProcess** pCpProcess)
{
	// ��ʼ��
	*pCpProcess = new GrayTransProcess();

	// ������ֵ
	(*pCpProcess)->fBottom = this->fBottom;
	(*pCpProcess)->fTop = this->fTop;
	(*pCpProcess)->fR = this->fR;

	if (false == pAreaSelect.isNull())
	{
		pAreaSelect.get()->Copy((*pCpProcess)->pAreaSelect);
	}

	return true;
}

/**************************************************************
�������� bIsValChanged

���� ��  �ж��Ƿ��������ı�

��ڲ��� ��

��������ֵ ��
	true  �� ����
	false �� δ����
****************************************************************/
bool GrayTransProcess::bIsValChanged()
{ 
	if ((0 == fBottom) && (255 == fTop) && (1.0 == fR))
	{
		return false;
	}
	else
	{
		return true;
	}
}
/*******************************  End:GrayTransProcess   ************************************/



/*******************************  Start:ImageProPipe   ************************************/
/**************************************************************
�������� ~ImageProPipe

���� ��  ��������

��ڲ��� ��

��������ֵ ��
	
****************************************************************/
ImageProPipe::~ImageProPipe()
{
	DeletePipe();
}

/**************************************************************
�������� GetPipeLen

���� ��  ��ȡ����ͨ������
         ����ȡ����ͨ��������ͼ��������

��ڲ��� ��

��������ֵ ��
	int Len �� ����ͨ������
****************************************************************/
int ImageProPipe::GetPipeLen()
{
	return PipeList.length();
}

/**************************************************************
�������� GetPipeLen

���� ��  ���ͼ������������ͨ��
	
��ڲ��� ��
	ProcessType InType ��        ��������
	ImageProcess* pImageProcess  ͼ������ָ��

��������ֵ ��
	int Err �� 0      -- ���
	           others -- δ���
****************************************************************/
int ImageProPipe::PipeAppend
(
	ProcessType InType,
	ImageProcess* pImageProcess
)
{
	// ��������
	bool bRet;
	QPair<ProcessType, ImageProcess*> ProcessPair;

	// ��ֵ
	ProcessPair.first = InType;
	ProcessPair.second = pImageProcess;

	// ����LIST
	PipeList.append(ProcessPair);

	return SUCCESS;
}

/**************************************************************
�������� PipeGet

���� ��  ��ȡ������ͨ���е�ĳ������ʵ��

��ڲ��� ��
	ProcessType InType ��        ��������
	ImageProcess* pImageProcess  ͼ������ָ��

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
int ImageProPipe::PipeGet
(
	int iNo,
	ProcessType &InType,
	ImageProcess **pImageProcess
)
{
	// ��������
	QPair<ProcessType, ImageProcess*> ProcessPair;

	// ��μ��
	if ((iNo < 0) || (iNo >= PipeList.length()))
		return ERR_INPUT;

	// ��ֵ
	ProcessPair = PipeList[iNo];
	InType = ProcessPair.first;
	*pImageProcess = ProcessPair.second;

	return SUCCESS;
}

/**************************************************************
�������� ProPipeCut

���� ��  �Ӵ���ܵ���ĳ�����н�ȡ
         ��������ѡȡλ������֮���ͼ����Ӵ���ܵ���ɾ��

��ڲ��� ��
	int iProCutNo ��  ��ȡλ��

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
int ImageProPipe::ProPipeCut(int iProCutNo)
{
	// ��������
	int iRet;
	int iLen;

	// ���Ȼ�ȡ�볤���ж�
	iLen = GetPipeLen();
	if (0 >= iLen) return ERR_INER_PARA;

	// ����ж�
	if ((0 > iProCutNo) || (iLen <= iProCutNo)) return ERR_INPUT;

	// ����ܵ���ȡ
	while ((PipeList.size() - 1) >= iProCutNo)
	{
		// ɾ������ܵ�ĩβ��һ������
		iRet = DeletePro((PipeList.size() - 1));
		if (SUCCESS != iRet) return ERR_FUNC;
	}

	return SUCCESS;
}

/**************************************************************
�������� DeletePro

���� ��  ɾ������ʽ

��ڲ��� ��
	int iProNo ��  ��ɾ���Ĵ���ʽ���

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
int ImageProPipe::DeletePro(int iProNo)
{
	// ��������
	int iLen;

	// ���Ȼ�ȡ�볤���ж�
	iLen = GetPipeLen();
	if (0 >= iLen) return ERR_INER_PARA;

	// ����ж�
	if ((0 > iProNo) || (iLen <= iProNo)) return ERR_INPUT;

	// ɾ����Ӧ��ָ������
	DeleteProPointer(PipeList[iProNo]);

	// �Ӷ�����ɾ������ʽ
	PipeList.removeAt(iProNo);

	return SUCCESS;
}

/**************************************************************
�������� Copy

���� ��  ��������

��ڲ��� ��
	QSharedPointer<ImageProPipe>& pCpPipe ��������ָ��

��������ֵ ��
	true  �� ��ȷ
	false �� ����
****************************************************************/
bool ImageProPipe::Copy(QSharedPointer<ImageProPipe> &pCpPipe)
{
	// ��������
	int iPipeSize;
	ProcessType m_PType;
	ImageProcess* pImgProcess;
	ImageProcess* pImgCpProcess;
	GrayTransProcess *pGrayTransPro;
	GrayTransProcess *pGrayTransCpPro;

	// ��ʼ��
	pCpPipe.reset(new ImageProPipe);

	// �ܵ�����
	iPipeSize = this->GetPipeLen();
	for (int i = 0; i < iPipeSize; i++)
	{
		// ��ȡ�ܵ��еĵ���������
		this->PipeGet(i, m_PType, &pImgProcess);
		
		// ���ƴ���ָ��ֵ
		switch (m_PType)
		{
			case ProcessType_GrayTrans:
			{ 
				pGrayTransPro = (GrayTransProcess*)pImgProcess;
				pGrayTransPro->Copy(&pGrayTransCpPro);
				pImgCpProcess = (ImageProcess*)pGrayTransCpPro;
				break;
			}
			case ProcessType_Enhancement:
			case ProcessType_None:
			default:
			{
				return false;
			}
		}

		// ���봦��ܵ�
		pCpPipe.get()->PipeAppend(m_PType, pImgCpProcess);
	}
	
	return true;
}

/**************************************************************
�������� DeletePipe

���� ��  ɾ������ܵ�

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ImageProPipe::DeletePipe()
{
	// ��������
	int iLength;

	// ɾ������ܵ�
	iLength = GetPipeLen();
	for (int i = 0; i < iLength; i++)
	{
		DeleteProPointer(PipeList[i]);
	}

	// �������ܵ�
	PipeList.clear();

	return;
}

/**************************************************************
�������� DeleteProPointer

���� ��  ɾ������ʽָ��

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ImageProPipe::DeleteProPointer(QPair<ProcessType, ImageProcess*> PairProcess)
{
	// �жϴ�����
	switch (PairProcess.first)
	{
		case ProcessType_GrayTrans:		// �Ҷȱ任����
		{
			delete (GrayTransProcess *)PairProcess.second;

			break;
		}
		case ProcessType_Enhancement:	// ͼ����ǿ����
		{

			break;
		}
		default:
		{
			delete PairProcess.second;

			break;
		}
	}
	
	return;
}
/*******************************  End:ImageProPipe   ************************************/




/*******************************  Start:ImageProcessInstance   ************************************/
/**************************************************************
�������� ImageProcessInstance

���� ��  ���캯��

��ڲ��� ��

��������ֵ ��

****************************************************************/
ImageProcessInstance::ImageProcessInstance()
{
	pProcessPipe.reset(new ImageProPipe);
	
	PairCurrProcess.first = ProcessType_None;
	PairCurrProcess.second = nullptr;
}

/**************************************************************
�������� Copy

���� ��  ��������

��ڲ��� ��
	QSharedPointer<ImageProcessInstance>& pCpImageProcessIns ��������ָ��

��������ֵ ��
	true  �� ��ȷ
	false �� ����
****************************************************************/
bool ImageProcessInstance::Copy
(
	QSharedPointer<ImageProcessInstance>& pCpImageProcessIns
)
{
	// ��������
	ImageProcess *pImgProcess;
	GrayTransProcess *pGrayTransPro;
	GrayTransProcess *pGrayTransCpPro;

	// ��ʼ��
	pCpImageProcessIns.reset(new ImageProcessInstance);

	// ����ܵ�����
	this->pProcessPipe.get()->Copy(pCpImageProcessIns.get()->pProcessPipe);
	
	// ��ǰ������
	pCpImageProcessIns.get()->PairCurrProcess.first = this->PairCurrProcess.first;
	switch (PairCurrProcess.first)
	{
		case ProcessType_GrayTrans:
		{
			pGrayTransPro = (GrayTransProcess*)this->PairCurrProcess.second;
			pGrayTransPro->Copy(&pGrayTransCpPro);
			pCpImageProcessIns.get()->PairCurrProcess.second 
				= (ImageProcess*)pGrayTransCpPro;
			break;
		}
		case ProcessType_Enhancement:
		case ProcessType_None:
		default:
		{
			return false;
		}
	}

	return true;
}

/**************************************************************
�������� AddCurrProcessToPipe

���� ��  ����ǰͼ�������ͼ����ܵ�

��ڲ��� ��

��������ֵ ��
	int Err �� 0      -- ���
	           others -- δ���
****************************************************************/
int ImageProcessInstance::AddCurrProcessToPipe()
{
	// ��������
	int iRet;      // ����ֵ
	bool bRet;     // ����ֵ
	
	// �жϵ�ǰͼ�����Ƿ����
	bRet = IsCurrProcessValid();
	if (true != bRet) return ERR_NO_CURR_PROCESS;
	
	// ����ǰ�������봦��ܵ�
	iRet = pProcessPipe.get()->PipeAppend(
		PairCurrProcess.first, 
		PairCurrProcess.second);
	if (SUCCESS != iRet) return ERR_FUNC;

	// ��ǰ�������
	PairCurrProcess.first = ProcessType_None;
	PairCurrProcess.second = nullptr;

	return SUCCESS;
}


/**************************************************************
�������� IsCurrProcessValid

���� ��  ��ǰͼ�����Ƿ����

��ڲ��� ��

��������ֵ ��
	true  �� ��ȷ
	false �� ����
****************************************************************/
bool ImageProcessInstance::IsCurrProcessValid()
{
	if ((ProcessType_None == PairCurrProcess.first)
		|| (nullptr == PairCurrProcess.second))
	{
		return false;
	}
	else return true;
}

/**************************************************************
�������� IsProcessPipeValid

���� ��	 ��ǰͼ����ܵ��Ƿ����

��ڲ��� �� 

��������ֵ ��
	true  �� ��ȷ
	false �� ����
****************************************************************/
bool ImageProcessInstance::IsProcessPipeValid()
{
	// ����ܵ����Ƿ����
	if (true == pProcessPipe.isNull()) return false;
	
	// ����ܵ������ж�
	if (0 == pProcessPipe.get()->GetPipeLen()) return false;

	return true;
}

/**************************************************************
�������� ClearCurrProcess

���� ��  �����ǰ����

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ImageProcessInstance::ClearCurrProcess()
{
	// ��ǰ�������ͼ��
	if (ProcessType_None == PairCurrProcess.first) return;

	// 
	switch (PairCurrProcess.first)
	{
		case ProcessType_GrayTrans:
		{
			delete (GrayTransProcess*)PairCurrProcess.second;
			PairCurrProcess.first = ProcessType_None;

			break;
		}

		case ProcessType_Enhancement:
		{
			break;
		}
		case ProcessType_None:

		default:
		{
			break;
		}
	}

	return;
}

/**************************************************************
�������� ClearProPipe

���� ��  �������

��ڲ��� ��

��������ֵ ��

****************************************************************/
void ImageProcessInstance::ClearProPipe()
{
	pProcessPipe.get()->DeletePipe();
}


/**************************************************************
�������� LoadImageProPipe

���� ��  ����ͼ����ܵ�

��ڲ��� ��
	QSharedPointer<ImageProPipe> pInProcessPipe

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
int ImageProcessInstance::LoadImageProPipe
(
	QSharedPointer<ImageProPipe> pInProcessPipe
)
{
	// ��������
	bool bRet;
	
	// ��μ��
	if (true == pInProcessPipe.isNull()) return ERR_INPUT;

	// ����ܵ���ֵ
	bRet = pInProcessPipe.get()->Copy(pProcessPipe);

	return SUCCESS;
}

/**************************************************************
�������� ProPipeExcute

���� ��  ͼ����ܵ�ִ��

��ڲ��� ��
	QSharedPointer<float> pInData ��	
	QSharedPointer<float> pOutData ��	
	int iWidth ��						
	int iHeight ��						

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
int ImageProcessInstance::ProPipeExcute
(
	QSharedPointer<float> pInData,
	QSharedPointer<float> pOutData,
	int iWidth,
	int iHeight
	
)
{
	//  ��������
	int iRet;
	int ProPipSize;
	ProcessType InType;
	QSharedPointer<float> pTmpInData;
	QSharedPointer<float> pTmpOutData;
	ImageProcess *pImgPro;

	// �ܵ�ָ����
	if (true == pProcessPipe.isNull()) return SUCCESS;

	// ��ȡ�ܵ��ߴ�
	ProPipSize = pProcessPipe.get()->GetPipeLen();
	if (0 == ProPipSize) return SUCCESS;

	// ��ʼ�� 
	pTmpInData.reset(new float[iHeight * iWidth]);
	pTmpOutData.reset(new float[iHeight * iWidth]);
	
	// ��ֵ
	memcpy_s(pTmpInData.get(), iHeight * iWidth * sizeof(float),
		pInData.get(), iHeight * iWidth * sizeof(float));

	// ���йܵ�����
	for (int iProNo = 0; iProNo < ProPipSize; iProNo++)
	{
		// ��ȡ��ǰ����
		iRet = pProcessPipe.get()->PipeGet(iProNo, InType, &pImgPro);
		if (SUCCESS != iRet) return ERR_FUNC;
		
		// ͼ����
		iRet = pImgPro->ImagePro(pTmpInData, pTmpOutData, iWidth, iHeight);
		if (SUCCESS != iRet) return ERR_FUNC;

	    // ����Դ���ݸ�ֵ
		memcpy_s(pTmpInData.get(), iHeight * iWidth * sizeof(float),
			pTmpOutData.get(), iHeight * iWidth * sizeof(float));
		
	}

	// ������ݸ�ֵ
	memcpy_s(pOutData.get(), iHeight * iWidth * sizeof(float),
		pTmpOutData.get(), iHeight * iWidth * sizeof(float));

	return SUCCESS;
}

/**************************************************************
�������� ProPipeExcute

���� ��  ͼ����ܵ�ִ��

��ڲ��� ��
	QSharedPointer<float> pInData ��
	QSharedPointer<float> pOutData ��
	int iWidth ��
	int iHeight ��

��������ֵ ��
	int Err �� 0      -- ���
			   others -- δ���
****************************************************************/
int ImageProcessInstance::ProPipeCut(int iCutProNo)
{
	// ��������
	int iRet;

	// ����ܵ�ָ����
	if (true == pProcessPipe.isNull()) return ERR_INER_POINTER;

	// ����ܵ���ȡ
	iRet = pProcessPipe.get()->ProPipeCut(iCutProNo);
	if (SUCCESS != iRet) return ERR_FUNC;
	
	return SUCCESS;
}

/*******************************  End:ImageProcessInstance   ************************************/
