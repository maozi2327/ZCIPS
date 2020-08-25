#include "ImageData.h"
#include "FreeImage.h"
#include "ERRInfo.h"

// 添加lib文件
#pragma comment(lib, "FreeImage.lib")

/**************************************************************
函数名： ZCIPSData

定义 ：  构造函数

入口参数 ：

函数返回值 ：

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
函数名： InitData

定义 ：  初始化数据

入口参数 ：
	int iInHeight ： 图像高度
	int iInWidth ：  图像宽度

函数返回值 ：
	true ：		成功
	false ：	失败
****************************************************************/
bool ImageData::InitData
(
	int iInHeight,
	int iInWidth
)
{
	// 入参检查
	if ((0 > iInHeight) || (0 > iInWidth))
	{
		return false;
	}

	// 初始化
	iHeight = iInHeight;
	iWidth = iInWidth;
	pfOrgData.reset(new float[iInHeight * iInWidth]);
	pfCurrData.reset(new float[iInHeight * iInWidth]);
	pfProcessingData.reset(new float[iInHeight * iInWidth]);

	return true;
}


/**************************************************************
函数名： ClearData

定义 ：  清除数据

入口参数 ：

函数返回值 ：

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
函数名： GetDataFromFile

定义 ：  从文件中加载数据

入口参数 ：
	QString StrFileName ：	

函数返回值 ：
	true ：		成功
	false ：	失败
****************************************************************/
bool ImageData::GetDataFromFile(QString StrFileName)
{
	// 参数声明
	bool bRet;                     // 函数返回值
	int iHeight;                   // 图像高度
	int iWidth;                    // 图像宽度
	int iPixBits;                  // 像素结构单元
	FREE_IMAGE_FORMAT FIFormat;    // 图片类型
	FIBITMAP *FIBmp;               // 加载的图像
	unsigned char *pucDataPt;
	unsigned short *pusDataPt;

	// 初始化
	FIBmp = nullptr;

	// 获取图片类型
	FIFormat = FreeImage_GetFileTypeU
	(reinterpret_cast<const wchar_t *>(StrFileName.utf16()));
	
	// 加载图像并获取参数
	FIBmp = FreeImage_LoadU(FIFormat, reinterpret_cast<const wchar_t *>(StrFileName.utf16()));
	if (nullptr == FIBmp) return false;
	iPixBits = FreeImage_GetBPP(FIBmp);
	iHeight = FreeImage_GetHeight(FIBmp);
	iWidth = FreeImage_GetWidth(FIBmp);

	// 初始化
	bRet = InitData(iHeight, iWidth);
	if (false == bRet) return false;

	// 加载数据
	switch (iPixBits)
	{
		// 8位数据
		case 8:
		{
			// 获取数据指针
			pucDataPt = FreeImage_GetBits(FIBmp);

			// 数据赋值
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
		// 16位数据
		case 16:
		{
			// 获取数据指针
			pusDataPt = (unsigned short*)FreeImage_GetBits(FIBmp);

			// 数据赋值
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
		// 24位数据
		case 24:
		{
			// 获取数据指针
			pucDataPt = FreeImage_GetBits(FIBmp);

			// RGB三色转灰度值
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
		// 32位数据
		case 32:
		{
			// 获取数据指针
			pucDataPt = FreeImage_GetBits(FIBmp);

			// RGB三色转灰度值
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
函数名： ProcessingAccept

定义 ：  正在处理的图像获得接受

入口参数 ：

函数返回值 ：
	int iRet ：
****************************************************************/
int ImageData::ProcessingAccept()
{
	// 处理中数据检查
	if (true == pfProcessingData.isNull()) return ERR_INPUT;
	
	// 初始化
	pfCurrData.reset(new float[iHeight * iWidth]);

	// 将处理中的图像数据赋值给当前处理数据
	memcpy_s(pfCurrData.get(), iHeight * iWidth * sizeof(float),
		pfProcessingData.get(), iHeight * iWidth * sizeof(float));

	return SUCCESS;
}

/**************************************************************
函数名： ProcessingCancel

定义 ：  正在处理的图像被撤销

入口参数 ：

函数返回值 ：
	int iRet ：
****************************************************************/
int ImageData::ProcessingCancel()
{
	// 处理中数据检查
	if (true == pfCurrData.isNull()) return ERR_INPUT;

	// 初始化
	pfProcessingData.reset(new float[iHeight * iWidth]);

	// 将处理中的图像数据赋值给当前处理数据
	memcpy_s(pfProcessingData.get(), iHeight * iWidth * sizeof(float),
		pfCurrData.get(), iHeight * iWidth * sizeof(float));

	return SUCCESS;
}

/**************************************************************
函数名： ResetData

定义 ：  数据重置
         即重置为原始数据

入口参数 ：

函数返回值 ：
	int iRet ：
****************************************************************/
int ImageData::ResetData()
{
	// 源数据检查
	if (true == pfOrgData.isNull()) return ERR_INPUT;

	// 初始化
	pfProcessingData.reset(new float[iHeight * iWidth]);
	pfCurrData.reset(new float[iHeight * iWidth]);

	// 数据赋值
	memcpy_s(pfCurrData.get(), iHeight * iWidth * sizeof(float),
		pfOrgData.get(), iHeight * iWidth * sizeof(float));
	memcpy_s(pfProcessingData.get(), iHeight * iWidth * sizeof(float),
		pfOrgData.get(), iHeight * iWidth * sizeof(float));

	return SUCCESS;
}

/**************************************************************
函数名： Copy

定义 ：  拷贝函数

入口参数 ：
	QSharedPointer<ImageData> pCpImageData ： 拷贝指针

函数返回值 ：
	true  ： 成功
	false ： 失败
****************************************************************/
bool ImageData::Copy(QSharedPointer<ImageData>& pCpImageData)
{
	// 参数声明
	int iHeight;
	int iWidth;
	
	// 初始化
	pCpImageData.reset(new ImageData);

	// 赋值
	iHeight = this->iHeight;
	iWidth = this->iWidth;

	// 赋值
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