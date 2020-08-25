#pragma once

#include "ImageData.h"
#include "ImageProcess.h"
#include "ImageMark.h"

class ImageInstance
{

// 参数
public:
	QSharedPointer<ImageProcessInstance> pImageProcessIns;      // 图像处理实例
																// 包含：
																// 1 -- 图像处理管道
																// 2 -- 当前处理

	// QSharedPointer<Marker> pMarker;

	QSharedPointer<ImageData> pImageData;						// 图像数据
	QSharedPointer<ImageMark>pImageMarks;                       //图像标记

	ImageMouseType mouseType;                                   //判断当前鼠标是否做标记以及做何种标记

// 响应与重载
public:
	ImageInstance();

// 函数
public:	
	// 重文件中加载图像实例
	int LoadFromFile(QString StrInPath);

	// 将当前处理加入处理管道
	int AddProcessPipe();

	// 当前图像处理获得接受，数据发生改变
	int DataProccessAccept();

	// 当前图像处理是否存在
	bool IsCurrProcessValid();

	// 当前图像处理管道是否存在
	bool IsProcessPipeValid();

	// 复制
	bool Copy(QSharedPointer<ImageInstance>& pImageIns);

	// 载入新的图像处理管道
	int LoadImageProPipe(QSharedPointer<ImageProPipe> pProcessPipe);

	// 处理管道执行
	int ProPipeExcute();

	// 处理管道截取
	int ProPipeCut(int iCutProNo);

	// 处理管道长度获取
	int ProPipeSize();

	// 清除当前处理
	void ClearCurrProcess();

	// 清除处理管道
	void ClearProPipe();

	// 数据重置
	void ResetImgData();
};


class ImageInstanceMap
{
public:

	QMap<QString, QSharedPointer<ImageInstance>> MapImageIns;	// 图像实例Map
															    // QString -- 图像标识
															    // ImageInstance -- 图像实例

	QList<QString> SortList;                                    // 因为Map无法排序，
	                                                            // 加入一个List容器来排序

	

public:

	// 获取Map的长度
	int Size();

	// 将图像实例插入Map
	void Insert(QString StrLabel, QSharedPointer<ImageInstance> pImageIns);

	//将图像实例从Map中删除
	void Delete(QString StrLabel);

	// 获取对应编号的Key值
	QString KeyAt(int iNo);

	// 获取对应编号的Value值
	QSharedPointer<ImageInstance> ValueAt(int iNo);

	// Value值搜寻
	QSharedPointer<ImageInstance> Find(QString StrKey);
};