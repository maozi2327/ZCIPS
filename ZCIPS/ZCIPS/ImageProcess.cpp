#include <opencv2/opencv.hpp>
#include <math.h>
#include "ImageProcess.h"
#include "ERRInfo.h"


// 添加lib文件
#pragma comment(lib, OPENCV_LIB)

using namespace std;
using namespace cv;

/*******************************  Start:GrayTransProcess   ************************************/

/**************************************************************
函数名： GrayTransProcess

定义 ：  构造函数

入口参数 ：

函数返回值 ：

****************************************************************/
GrayTransProcess::GrayTransProcess()
{
	fBottom = (float)0;
	fTop = (float)255;
	fR = (float)1;
	pAreaSelect.clear();
}

/**************************************************************
函数名： ImagePro

定义 ：  图像处理

入口参数 ：
	QSharedPointer<float> pfSrc ： 
	QSharedPointer<float> pfDes ：
	int iWidth ： 
	int iHeight ：

函数返回值 ：
	int iRet ：
****************************************************************/
int GrayTransProcess::ImagePro
(
	QSharedPointer<float> pfSrc,
	QSharedPointer<float> pfDes,
	int iWidth, 
	int iHeight
)
{
	// 参数声明
	Mat MatProcess;

	// 入参检查
	if ((true == pfSrc.isNull()) || (true == pfDes.isNull()))
		return ERR_INPUT;

	// 数据矩阵初始化
	MatProcess = Mat(iHeight, iWidth, CV_32F);
	for (int i = 0; i < iHeight; i++)
		for (int j = 0; j < iWidth; j++)
			MatProcess.at<float>(i, j) = pfSrc.get()[i * iWidth + j];
	
	//// 测试代码
	//Mat MatTest = Mat(iHeight, iWidth, CV_8U);
	//for (int i = 0; i < iHeight; i++)
	//	for (int j = 0; j < iWidth; j++)
	//	{
	//		MatTest.at<UCHAR>(i, j) = (UCHAR)MatProcess.at<float>(i, j);
	//	}
	//imwrite("D:\\a.png", MatTest);

	// 进行灰度线性变换
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
	
	// 进行灰度指数变换
	pow(MatProcess / 255, (double)fR, MatProcess);
	MatProcess = MatProcess * 255;

	// 数据输出
	if (false == pAreaSelect.isNull())  // 区域选择
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
	else	// 非区域选择
	{
		memcpy_s(pfDes.get(), iHeight * iWidth * sizeof(float),
			MatProcess.ptr(0), iHeight * iWidth * sizeof(float));
	}

	return SUCCESS;
}

/**************************************************************
函数名： SetPara

定义 ：  参数设置

入口参数 ：
	float fInBottom,
	float fInTop, 
	float fR

函数返回值 ：

****************************************************************/
void GrayTransProcess::SetPara(float fInBottom, float fInTop, float fInR)
{
	SetBottom(fInBottom);
	SetTop(fInTop);
	SetR(fInR);
}

/**************************************************************
函数名： SetBottom

定义 ：  下线设置

入口参数 ：
	float fInBottom

函数返回值 ：

****************************************************************/
void GrayTransProcess::SetBottom(float fInBottom)
{
	fBottom = fInBottom;
}

/**************************************************************
函数名： SetTop

定义 ：  上线设置

入口参数 ：
	float fInTop

函数返回值 ：

****************************************************************/
void GrayTransProcess::SetTop(float fInTop)
{
	fTop = fInTop;
}

/**************************************************************
函数名： SetR

定义 ：  R设置

入口参数 ：
	float fInR

函数返回值 ：

****************************************************************/
void GrayTransProcess::SetR(float fInR)
{
	fR = fInR;
}

/**************************************************************
函数名： GetBottom

定义 ：  获取Bottom值

入口参数 ：
	
函数返回值 ：
	float fBottom ：下限值
****************************************************************/
float GrayTransProcess::GetBottom() { return fBottom; }

/**************************************************************
函数名： GetBottom

定义 ：  获取Bottom值

入口参数 ：

函数返回值 ：
	float fR ：R值
****************************************************************/
float GrayTransProcess::GetR() { return fR; }

/**************************************************************
函数名： GetBottom

定义 ：  获取Bottom值

入口参数 ：

函数返回值 ：
	float fTop ：上限值
****************************************************************/
float GrayTransProcess::GetTop() { return fTop; }

/**************************************************************
函数名： setArea

定义 ：  处理区域设置

入口参数 ：
	QSharedPointer<AreaSelect> pInAreaSel

函数返回值 ：

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
函数名： GetParaString

定义 ：  获取参数字符

入口参数 ：
	QString &StrPara

函数返回值 ：
	
****************************************************************/
int GrayTransProcess::GetParaString(QString &StrPara)
{
	// 参数声明
	QTextCodec* pTCode;							// 字符类型变换指针

	// 初始化
	StrPara.clear();

	// 获取文件对话框
	pTCode = QTextCodec::codecForName("GBK");
	StrPara = pTCode->toUnicode("下限值: ") + QString::number((double)fBottom, 'g', 3) + '\n'
		+ pTCode->toUnicode("上限值: ") + QString::number((double)fTop, 'g', 3) + '\n'
		+ pTCode->toUnicode("指数值: ") + QString::number((double)fR, 'g', 3);
	if (false == pAreaSelect.isNull())
	{
		StrPara += '\n' + pTCode->toUnicode("区域截取");
	}
		
	return SUCCESS;
}

/**************************************************************
函数名： Copy

定义 ：  拷贝函数

入口参数 ：
	GrayTransProcess** pCpProcess ：拷贝的指针

函数返回值 ：
	true  ： 正确
	false ： 错误
****************************************************************/
bool GrayTransProcess::Copy(GrayTransProcess** pCpProcess)
{
	// 初始化
	*pCpProcess = new GrayTransProcess();

	// 参数赋值
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
函数名： bIsValChanged

定义 ：  判断是否发生参数改变

入口参数 ：

函数返回值 ：
	true  ： 发生
	false ： 未发生
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
函数名： ~ImageProPipe

定义 ：  析构函数

入口参数 ：

函数返回值 ：
	
****************************************************************/
ImageProPipe::~ImageProPipe()
{
	DeletePipe();
}

/**************************************************************
函数名： GetPipeLen

定义 ：  获取处理通道长度
         即获取处理通道包含的图像处理步骤数

入口参数 ：

函数返回值 ：
	int Len ： 处理通道长度
****************************************************************/
int ImageProPipe::GetPipeLen()
{
	return PipeList.length();
}

/**************************************************************
函数名： GetPipeLen

定义 ：  添加图像处理步骤至处理通道
	
入口参数 ：
	ProcessType InType ：        处理种类
	ImageProcess* pImageProcess  图像处理类指针

函数返回值 ：
	int Err ： 0      -- 完成
	           others -- 未完成
****************************************************************/
int ImageProPipe::PipeAppend
(
	ProcessType InType,
	ImageProcess* pImageProcess
)
{
	// 参数声明
	bool bRet;
	QPair<ProcessType, ImageProcess*> ProcessPair;

	// 赋值
	ProcessPair.first = InType;
	ProcessPair.second = pImageProcess;

	// 加入LIST
	PipeList.append(ProcessPair);

	return SUCCESS;
}

/**************************************************************
函数名： PipeGet

定义 ：  获取处理方法通道中的某个处理实例

入口参数 ：
	ProcessType InType ：        处理种类
	ImageProcess* pImageProcess  图像处理类指针

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
int ImageProPipe::PipeGet
(
	int iNo,
	ProcessType &InType,
	ImageProcess **pImageProcess
)
{
	// 参数声明
	QPair<ProcessType, ImageProcess*> ProcessPair;

	// 入参检查
	if ((iNo < 0) || (iNo >= PipeList.length()))
		return ERR_INPUT;

	// 赋值
	ProcessPair = PipeList[iNo];
	InType = ProcessPair.first;
	*pImageProcess = ProcessPair.second;

	return SUCCESS;
}

/**************************************************************
函数名： ProPipeCut

定义 ：  从处理管道的某处进行截取
         即将包括选取位置与其之后的图像处理从处理管道中删除

入口参数 ：
	int iProCutNo ：  截取位置

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
int ImageProPipe::ProPipeCut(int iProCutNo)
{
	// 参数声明
	int iRet;
	int iLen;

	// 长度获取与长度判断
	iLen = GetPipeLen();
	if (0 >= iLen) return ERR_INER_PARA;

	// 入参判断
	if ((0 > iProCutNo) || (iLen <= iProCutNo)) return ERR_INPUT;

	// 处理管道截取
	while ((PipeList.size() - 1) >= iProCutNo)
	{
		// 删除处理管道末尾的一个处理
		iRet = DeletePro((PipeList.size() - 1));
		if (SUCCESS != iRet) return ERR_FUNC;
	}

	return SUCCESS;
}

/**************************************************************
函数名： DeletePro

定义 ：  删除处理方式

入口参数 ：
	int iProNo ：  待删除的处理方式编号

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
int ImageProPipe::DeletePro(int iProNo)
{
	// 参数声明
	int iLen;

	// 长度获取与长度判断
	iLen = GetPipeLen();
	if (0 >= iLen) return ERR_INER_PARA;

	// 入参判断
	if ((0 > iProNo) || (iLen <= iProNo)) return ERR_INPUT;

	// 删除对应的指针数据
	DeleteProPointer(PipeList[iProNo]);

	// 从队列中删除处理方式
	PipeList.removeAt(iProNo);

	return SUCCESS;
}

/**************************************************************
函数名： Copy

定义 ：  拷贝函数

入口参数 ：
	QSharedPointer<ImageProPipe>& pCpPipe ：拷贝的指针

函数返回值 ：
	true  ： 正确
	false ： 错误
****************************************************************/
bool ImageProPipe::Copy(QSharedPointer<ImageProPipe> &pCpPipe)
{
	// 参数声明
	int iPipeSize;
	ProcessType m_PType;
	ImageProcess* pImgProcess;
	ImageProcess* pImgCpProcess;
	GrayTransProcess *pGrayTransPro;
	GrayTransProcess *pGrayTransCpPro;

	// 初始化
	pCpPipe.reset(new ImageProPipe);

	// 管道复制
	iPipeSize = this->GetPipeLen();
	for (int i = 0; i < iPipeSize; i++)
	{
		// 获取管道中的单个处理步骤
		this->PipeGet(i, m_PType, &pImgProcess);
		
		// 复制处理指针值
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

		// 载入处理管道
		pCpPipe.get()->PipeAppend(m_PType, pImgCpProcess);
	}
	
	return true;
}

/**************************************************************
函数名： DeletePipe

定义 ：  删除处理管道

入口参数 ：

函数返回值 ：

****************************************************************/
void ImageProPipe::DeletePipe()
{
	// 参数声明
	int iLength;

	// 删除处理管道
	iLength = GetPipeLen();
	for (int i = 0; i < iLength; i++)
	{
		DeleteProPointer(PipeList[i]);
	}

	// 清除处理管道
	PipeList.clear();

	return;
}

/**************************************************************
函数名： DeleteProPointer

定义 ：  删除处理方式指针

入口参数 ：

函数返回值 ：

****************************************************************/
void ImageProPipe::DeleteProPointer(QPair<ProcessType, ImageProcess*> PairProcess)
{
	// 判断处理类
	switch (PairProcess.first)
	{
		case ProcessType_GrayTrans:		// 灰度变换处理
		{
			delete (GrayTransProcess *)PairProcess.second;

			break;
		}
		case ProcessType_Enhancement:	// 图像增强处理
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
函数名： ImageProcessInstance

定义 ：  构造函数

入口参数 ：

函数返回值 ：

****************************************************************/
ImageProcessInstance::ImageProcessInstance()
{
	pProcessPipe.reset(new ImageProPipe);
	
	PairCurrProcess.first = ProcessType_None;
	PairCurrProcess.second = nullptr;
}

/**************************************************************
函数名： Copy

定义 ：  拷贝函数

入口参数 ：
	QSharedPointer<ImageProcessInstance>& pCpImageProcessIns ：拷贝的指针

函数返回值 ：
	true  ： 正确
	false ： 错误
****************************************************************/
bool ImageProcessInstance::Copy
(
	QSharedPointer<ImageProcessInstance>& pCpImageProcessIns
)
{
	// 参数声明
	ImageProcess *pImgProcess;
	GrayTransProcess *pGrayTransPro;
	GrayTransProcess *pGrayTransCpPro;

	// 初始化
	pCpImageProcessIns.reset(new ImageProcessInstance);

	// 处理管道拷贝
	this->pProcessPipe.get()->Copy(pCpImageProcessIns.get()->pProcessPipe);
	
	// 当前处理拷贝
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
函数名： AddCurrProcessToPipe

定义 ：  将当前图像处理加入图像处理管道

入口参数 ：

函数返回值 ：
	int Err ： 0      -- 完成
	           others -- 未完成
****************************************************************/
int ImageProcessInstance::AddCurrProcessToPipe()
{
	// 参数声明
	int iRet;      // 返回值
	bool bRet;     // 返回值
	
	// 判断当前图像处理是否存在
	bRet = IsCurrProcessValid();
	if (true != bRet) return ERR_NO_CURR_PROCESS;
	
	// 将当前处理载入处理管道
	iRet = pProcessPipe.get()->PipeAppend(
		PairCurrProcess.first, 
		PairCurrProcess.second);
	if (SUCCESS != iRet) return ERR_FUNC;

	// 当前处理空置
	PairCurrProcess.first = ProcessType_None;
	PairCurrProcess.second = nullptr;

	return SUCCESS;
}


/**************************************************************
函数名： IsCurrProcessValid

定义 ：  当前图像处理是否存在

入口参数 ：

函数返回值 ：
	true  ： 正确
	false ： 错误
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
函数名： IsProcessPipeValid

定义 ：	 当前图像处理管道是否存在

入口参数 ： 

函数返回值 ：
	true  ： 正确
	false ： 错误
****************************************************************/
bool ImageProcessInstance::IsProcessPipeValid()
{
	// 处理管道类是否存在
	if (true == pProcessPipe.isNull()) return false;
	
	// 处理管道长度判断
	if (0 == pProcessPipe.get()->GetPipeLen()) return false;

	return true;
}

/**************************************************************
函数名： ClearCurrProcess

定义 ：  清除当前处理

入口参数 ：

函数返回值 ：

****************************************************************/
void ImageProcessInstance::ClearCurrProcess()
{
	// 当前处理类型检查
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
函数名： ClearProPipe

定义 ：  清除处理

入口参数 ：

函数返回值 ：

****************************************************************/
void ImageProcessInstance::ClearProPipe()
{
	pProcessPipe.get()->DeletePipe();
}


/**************************************************************
函数名： LoadImageProPipe

定义 ：  载入图像处理管道

入口参数 ：
	QSharedPointer<ImageProPipe> pInProcessPipe

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
int ImageProcessInstance::LoadImageProPipe
(
	QSharedPointer<ImageProPipe> pInProcessPipe
)
{
	// 参数声明
	bool bRet;
	
	// 入参检查
	if (true == pInProcessPipe.isNull()) return ERR_INPUT;

	// 处理管道赋值
	bRet = pInProcessPipe.get()->Copy(pProcessPipe);

	return SUCCESS;
}

/**************************************************************
函数名： ProPipeExcute

定义 ：  图像处理管道执行

入口参数 ：
	QSharedPointer<float> pInData ：	
	QSharedPointer<float> pOutData ：	
	int iWidth ：						
	int iHeight ：						

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
int ImageProcessInstance::ProPipeExcute
(
	QSharedPointer<float> pInData,
	QSharedPointer<float> pOutData,
	int iWidth,
	int iHeight
	
)
{
	//  参数声明
	int iRet;
	int ProPipSize;
	ProcessType InType;
	QSharedPointer<float> pTmpInData;
	QSharedPointer<float> pTmpOutData;
	ImageProcess *pImgPro;

	// 管道指针检查
	if (true == pProcessPipe.isNull()) return SUCCESS;

	// 获取管道尺寸
	ProPipSize = pProcessPipe.get()->GetPipeLen();
	if (0 == ProPipSize) return SUCCESS;

	// 初始化 
	pTmpInData.reset(new float[iHeight * iWidth]);
	pTmpOutData.reset(new float[iHeight * iWidth]);
	
	// 赋值
	memcpy_s(pTmpInData.get(), iHeight * iWidth * sizeof(float),
		pInData.get(), iHeight * iWidth * sizeof(float));

	// 进行管道处理
	for (int iProNo = 0; iProNo < ProPipSize; iProNo++)
	{
		// 获取当前处理
		iRet = pProcessPipe.get()->PipeGet(iProNo, InType, &pImgPro);
		if (SUCCESS != iRet) return ERR_FUNC;
		
		// 图像处理
		iRet = pImgPro->ImagePro(pTmpInData, pTmpOutData, iWidth, iHeight);
		if (SUCCESS != iRet) return ERR_FUNC;

	    // 处理源数据赋值
		memcpy_s(pTmpInData.get(), iHeight * iWidth * sizeof(float),
			pTmpOutData.get(), iHeight * iWidth * sizeof(float));
		
	}

	// 结果数据赋值
	memcpy_s(pOutData.get(), iHeight * iWidth * sizeof(float),
		pTmpOutData.get(), iHeight * iWidth * sizeof(float));

	return SUCCESS;
}

/**************************************************************
函数名： ProPipeExcute

定义 ：  图像处理管道执行

入口参数 ：
	QSharedPointer<float> pInData ：
	QSharedPointer<float> pOutData ：
	int iWidth ：
	int iHeight ：

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
int ImageProcessInstance::ProPipeCut(int iCutProNo)
{
	// 参数声明
	int iRet;

	// 处理管道指针检查
	if (true == pProcessPipe.isNull()) return ERR_INER_POINTER;

	// 处理管道截取
	iRet = pProcessPipe.get()->ProPipeCut(iCutProNo);
	if (SUCCESS != iRet) return ERR_FUNC;
	
	return SUCCESS;
}

/*******************************  End:ImageProcessInstance   ************************************/
