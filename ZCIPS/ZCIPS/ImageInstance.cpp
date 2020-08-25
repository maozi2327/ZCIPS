
#include "ImageInstance.h"
#include "ERRInfo.h"
/**************************************************************
函数名： ImageInstance

定义 ：  构造函数

入口参数 ：

函数返回值 ：

****************************************************************/
ImageInstance::ImageInstance()
{
	pImageProcessIns.reset(new ImageProcessInstance);
	pImageData.reset(new ImageData);
	pImageMarks.reset(new ImageMark);
	mouseType = Mouse_Default;
}

/**************************************************************
函数名： LoadFromFile

定义 ：  重文件中加载图像实例

入口参数 ：
	QString StrPath ：  文件路径

函数返回值 ：
	int Err ： 0      -- 完成
	           others -- 未完成
****************************************************************/
int ImageInstance::LoadFromFile(QString StrInPath)
{
	// 参数声明
	int iRet;
	bool bRet;

	// 数据加载
	bRet = pImageData.get()->GetDataFromFile(StrInPath);
	if (false == bRet) return ERR_FUNC;

	return SUCCESS;
}

/**************************************************************
函数名： AddProcessPipe

定义 ：  将当前处理加入处理管道

入口参数 ：

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
int ImageInstance::AddProcessPipe()
{
	// 参数声明
	int iRet;
	bool bRet;
	
	// 当前处理检查
	bRet = pImageProcessIns.get()->IsCurrProcessValid();
	if (true != bRet) return ERR_NO_CURR_PROCESS;
	
	// 加入图像处理管道
	iRet = pImageProcessIns.get()->AddCurrProcessToPipe();
	if (SUCCESS != iRet) return ERR_FUNC;

	return SUCCESS;
}

/**************************************************************
函数名： DataProccessAccept

定义 ：  当前图像处理获得接受，
		 数据发生改变
		 即pfCurrData -> pfProcessingData

入口参数 ：

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
int ImageInstance::DataProccessAccept()
{
	// 参数声明
	int iRet;
	
	// pfCurrData -> pfProcessingData
	iRet = pImageData.get()->ProcessingAccept();
	if (SUCCESS != iRet) return ERR_FUNC;

	return SUCCESS;
}

/**************************************************************
函数名： IsCurrProcessValid

定义 ：  当前图像处理是否存在

入口参数 ：

函数返回值 ：
	true  ： 存在
	false ： 不存在
****************************************************************/
bool ImageInstance::IsCurrProcessValid()
{
	return pImageProcessIns.get()->IsCurrProcessValid();
}

/**************************************************************
函数名： IsCurrProcessValid

定义 ：  当前图像处理管道是否存在

入口参数 ：

函数返回值 ：
	true  ： 存在
	false ： 不存在
****************************************************************/
bool ImageInstance::IsProcessPipeValid()
{
	// 判断图像处理实例是否存在
	if (true == pImageProcessIns.isNull()) return false;
	
	// 判断图像处理管道
	if (false == pImageProcessIns.get()->IsProcessPipeValid()) return false;

	return true;
}

/**************************************************************
函数名： Copy

定义 ：  复制

入口参数 ：
	QSharedPointer<ImageInstance>& pImageIns ： 图像实例

函数返回值 ：
	true  ： 成功
	false ： 失败
****************************************************************/
bool ImageInstance::Copy(QSharedPointer<ImageInstance>& pImageIns)
{
	// 初始化
	pImageIns.reset(new ImageInstance);

	// 内部元素拷贝
	pImageProcessIns.get()->Copy(pImageIns.get()->pImageProcessIns);
	pImageData.get()->Copy(pImageIns.get()->pImageData);

	return true;
}

/**************************************************************
函数名： LoadImageProPipe

定义 ：  载入新的图像处理管道
		 并对当前图像数据执行该图像处理管道

入口参数 ：
	QSharedPointer<ImageProPipe> pInProcessPipe ：

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
int ImageInstance::LoadImageProPipe
(
	QSharedPointer<ImageProPipe> pInProcessPipe
)
{
	// 参数声明
	int iRet;
	
	// 入参检查
	if (true == pInProcessPipe.isNull()) return ERR_INPUT;

	// 载入图像处理管道
	iRet = pImageProcessIns.get()->LoadImageProPipe(pInProcessPipe);
	if (SUCCESS != iRet) return ERR_FUNC;

	// 图像管道执行
	iRet = ProPipeExcute();
	if (SUCCESS != iRet) return ERR_FUNC;

	return SUCCESS;
}

/**************************************************************
函数名： ProPipeExcute

定义 ：  执行图像处理管道

入口参数 ：

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
int ImageInstance::ProPipeExcute()
{
	// 参数声明
	int iRet;

	// 指针检测
	if ((true == pImageData.isNull()) || (true == pImageProcessIns.isNull()))
		return ERR_INPUT;

	// 数据重置
	iRet = pImageData.get()->ResetData();
	if (SUCCESS != iRet) return ERR_FUNC;

	// 执行图像处理管道
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
函数名： ProPipeCut

定义 ：  处理管道截取

入口参数 ：
	int iCutProNo ： 截取处理编号

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
int ImageInstance::ProPipeCut(int iCutProNo)
{
	// 参数声明
	int iRet;      // 函数返回值

	// 处理管道截取
	iRet = pImageProcessIns.get()->ProPipeCut(iCutProNo);
	if (SUCCESS != iRet) return ERR_FUNC;

	return SUCCESS;
}


/**************************************************************
函数名： ProPipeSize

定义 ：  处理管道长度获取

入口参数 ：

函数返回值 ：
	int iLen ： 处理管道长度
****************************************************************/
int ImageInstance::ProPipeSize()
{
	return pImageProcessIns.get()->pProcessPipe.get()->GetPipeLen();
}

/**************************************************************
函数名： ClearCurrProcess

定义 ：  清除当前处理

入口参数 ：

函数返回值 ：
	
****************************************************************/
void ImageInstance::ClearCurrProcess()
{
	pImageProcessIns.get()->ClearCurrProcess();
}

/**************************************************************
函数名： ClearProPipe

定义 ：  清除处理管道

入口参数 ：

函数返回值 ：

****************************************************************/
void ImageInstance::ClearProPipe()
{
	pImageProcessIns.get()->ClearProPipe();
}

/**************************************************************
函数名： ResetImgData

定义 ：  数据重置

入口参数 ：

函数返回值 ：

****************************************************************/
void ImageInstance::ResetImgData()
{
	pImageData.get()->ResetData();
}







/**************************************************************
函数名： Size

定义 ：  获取Map长度

入口参数 ：

函数返回值 ：
	int size ：Map长度值
****************************************************************/
int ImageInstanceMap::Size()
{
	return (int)SortList.size();
}

/**************************************************************
函数名： Insert

定义 ：  将图像实例插入图像Map

入口参数 ：

函数返回值 ：
	int Err ： 0      -- 完成
			   others -- 未完成
****************************************************************/
void ImageInstanceMap::Insert
(
	QString StrLabel,
	QSharedPointer<ImageInstance> pImageIns
)
{
	// 参数声明
	QMap<QString, QSharedPointer<ImageInstance>>::const_iterator it;    // QMap的迭代器

	// 检查是否有重复的图像实例
	it = MapImageIns.find(StrLabel);
	if (MapImageIns.end() != it) return;

	// 载入图像实例
	MapImageIns.insert(StrLabel, pImageIns);
	SortList.append(StrLabel);
}

/**************************************************************
函数名： KeyAt

定义 ：  依据排序编号获取Key值

入口参数 ：
	int iNo ：        编号

函数返回值 ：
	QString StrKey ： Key值
****************************************************************/
QString ImageInstanceMap::KeyAt(int iNo)
{
	// 参数声明
	QString StrKey;

	// 初始化
	StrKey.clear();

	// 编号判断
	if ((iNo < 0) || (iNo >= SortList.size())) return StrKey;

	// Key值赋值
	StrKey = SortList[iNo];

	return StrKey;
}

/**************************************************************
函数名： ValueAt

定义 ：  依据排序编号获取Value值

入口参数 ：
	int iNo ：        编号

函数返回值 ：
	QSharedPointer<ImageInstance>  ： Value值
****************************************************************/
QSharedPointer<ImageInstance> ImageInstanceMap::ValueAt(int iNo)
{
	// 参数声明
	QString StrKey;
	QSharedPointer<ImageInstance> pImageIns;
	QMap<QString, QSharedPointer<ImageInstance>>::iterator it;
	
	// 初始化
	pImageIns.clear();

	// 编号判断
	if ((iNo < 0) || (iNo >= SortList.size())) return pImageIns;

	// key值获取
	StrKey = SortList[iNo];

	// 迭代指针获取
	it = MapImageIns.find(StrKey);

	return it.value();
}

/**************************************************************
函数名： Find

定义 ：  依据key值寻找value值

入口参数 ：
	int iNo ：        编号

函数返回值 ：
	QSharedPointer<ImageInstance>  ： Value值
****************************************************************/
QSharedPointer<ImageInstance> ImageInstanceMap::Find(QString StrKey)
{
	// 参数声明
	QSharedPointer<ImageInstance> pImageIns;
	QMap<QString, QSharedPointer<ImageInstance>>::iterator it;

	// 初始化
	pImageIns.clear();

	// 迭代指针获取
	it = MapImageIns.find(StrKey);
	if (it == MapImageIns.cend()) return pImageIns;
	
	return it.value();
}
/***********************************************************************/
	//将图像实例从Map中删除
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