#pragma once

// Opencv库的宏定义
#ifdef _DEBUG
	#define OPENCV_LIB  "opencv_world410d.lib"
#else
	#define OPENCV_LIB  "opencv_world410.lib"
#endif

#include <QtWidgets>
#include <QSharedPointer>
#include "AreaSelect.h"

/***************************** 枚举类型 ******************************/
enum ProcessType
{
	ProcessType_None = 0,
	ProcessType_GrayTrans,
	ProcessType_Enhancement
};


/******************************** 类 **********************************/

// 图像处理（虚类）
class ImageProcess
{
public:
	virtual int ImagePro(QSharedPointer<float> pfSrc,
		QSharedPointer<float> pfDes, int iWidth, int iHeight) = 0;

	virtual int GetParaString(QString &StrPara) = 0;
};

// 灰度变换类
class GrayTransProcess : public ImageProcess
{

// 参数
private:
	float fBottom;     // 线性变换参数 -- 下线
	float fTop;        // 线性变换参数 -- 上线
	float fR;          // 指数变换参数 -- R值
	QSharedPointer<AreaSelect> pAreaSelect;   // 图像处理区域

// 函数
public:
	// 构造函数
	GrayTransProcess();

	// 图像处理
	int ImagePro
	(QSharedPointer<float> pfSrc, QSharedPointer<float> pfDes,
	 int iWidth, int iHeight) override;
	
	// 参数设置
	void SetPara(float fInBottom, float fInTop, float fInR);

	// 下限设置
	void SetBottom(float fInBottom);

	// 上限设置
	void SetTop(float fInTop);

	// R值设置
	void SetR(float fInR);

	// 获取值
	float GetBottom();

	// 获取值
	float GetTop();

	// 获取值
	float GetR();

	// 设置处理区域
	void setArea(QSharedPointer<AreaSelect> pInAreaSel);

	// 获取参数字符
	int GetParaString(QString &StrPara);

	// 复制
	bool Copy(GrayTransProcess** pCpProcess);

	// 判断是否发生参数改变
	bool bIsValChanged();

};

// 图像处理通道
class ImageProPipe
{
// 参数
private:
	QList<QPair<ProcessType, ImageProcess*>> PipeList;

// 函数
public:
	~ImageProPipe();

	// 获取长度
	int GetPipeLen();

	// 添加图像处理
	int PipeAppend(ProcessType InType, ImageProcess* pImageProcess);

	// 获取图像处理方法
	int PipeGet(int iNo, ProcessType &InType, ImageProcess **pImageProcess);

	// 处理管道截取
	int ProPipeCut(int iProCutNo);

	// 删除处理方式
	int DeletePro(int iProNo);

	// 复制函数
	bool Copy(QSharedPointer<ImageProPipe> &pCpPipe);

	// 删除处理管道
	void DeletePipe();

	// 删除处理方式指针
	void DeleteProPointer(QPair<ProcessType, ImageProcess*> PairProcess);

};

// 图像处理实例类
class ImageProcessInstance
{


// 参数
public:

	QSharedPointer<ImageProPipe> pProcessPipe;			 // 图像处理通道

	QPair<ProcessType, ImageProcess*> PairCurrProcess;   // 当前图像处理

// 响应与重载
public:
	ImageProcessInstance();

// 函数
public:
	bool Copy(QSharedPointer<ImageProcessInstance>& pCpImageProcessIns);

	int AddCurrProcessToPipe();

	bool IsCurrProcessValid();

	bool IsProcessPipeValid();

	void ClearCurrProcess();

	void ClearProPipe();

	int LoadImageProPipe(QSharedPointer<ImageProPipe> pInProcessPipe);

	int ProPipeExcute
	(
		QSharedPointer<float> pInData,
		QSharedPointer<float> pOutData,
		int iWidth,
		int iHeight
	);

	int ProPipeCut(int iCutProNo);
};