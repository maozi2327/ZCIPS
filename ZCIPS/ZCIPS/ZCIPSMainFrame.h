#pragma once

#include <QtWidgets>
#include "ui_ZCIPSMainFrame.h"
#include "ImageInstance.h"
#include "ImageDisplay.h"
#include "SubUI.h"
#include "AreaSelect.h"
#include "Marco.h"
#include "DataBaseProcess.h"

/***************************** 宏 ******************************/



/***************************** 枚举类型 ******************************/
enum DisplayMode
{
	DisplayMode_None = 0,	// 无显示
	DisplayMode_Detector,   // 探测器内容显示
	DisplayMode_ImageList	// 图像列表显示
};

enum DetectorNo
{
	Detector_1 = 1,     // 1号探测器
	Detector_2          // 2号探测器
};

enum PanelTab
{
	PanelTab_Control = 0,		// 控制界面
	PanelTab_ImagePro			// 图像处理界面
};

enum ImageProcessTab
{
	ImageProcessTab_GrayTrans = 0,		// 灰度变换界面
	ImageProcessTab_Enhancement			// 图像增强界面
};

/***************************** 类 ******************************/

class ZCIPSMainFrame : public QMainWindow
{
	Q_OBJECT

public:
	ZCIPSMainFrame(QWidget *parent = Q_NULLPTR);

private:
	Ui::ZCIPSMainFrameClass ui;
	
// 参数
public:
	QPair<QString, QSharedPointer<ImageInstance>> PairDetIns_1;		// 第一个探测器的图像实例
	
	QPair<QString, QSharedPointer<ImageInstance>> PairDetIns_2;		// 第二个探测器的图像实例
	
	ImageInstanceMap MapImageIns;									// 图像实例列表
																	// QString -- 图像标识
																	// ImageInstance -- 图像实例

	QPair<QString, QSharedPointer<ImageInstance>> PairCurrImageIns;   // 当前显示处理的图像实例
																	  // QString -- 图像标识
																	  // ImageInstance -- 图像实例
	
	QSharedPointer<ImageDisplay> pImageDisplay;		// 将图像实例中的图像数据与标记
													// 转换为显示的QWidget

	QSharedPointer<AreaSelect> pAreaSelect;			// 区域选择，该类用于软件图像的区域选择
													// 及其对应的区域处理处理
	
	QSharedPointer<Marco> pMarco;       // 宏处理

	bool bIsUpdatedDisplay;				// 是否更新图像显示标识位，
										// 该标志位用于避免因响应函数绑定引起的图像更新过于频繁
	
	bool bIsUpdateImageIns;             // 是否更新图像实例标识
	                                    // 避免因界面控制歧义导致图像实例被更改
	
	bool bIsUpdatePanel;                // 是否更新Panel界面标识
										// 避免因界面控制歧义导致Panel界面被更改
	
public slots:

	// 载入图像按钮响应槽函数
	void LoadImageFromFileSlot();

	// PanelTab 切换函数
	void SwitchPanelTab(int iIndex);

	// PanelTab 切换函数
	void SwitchImageProcessTab(int iIndex);

// 函数
public:

	// 整体界面初始化
	void InitialUI();

	// 更新Panel
	void UpdatePanel();

	// 将当前的图像处理载入图像处理管道
	void CurrProccessToPipe();

	// 判断当前显示的图像实例是否存在
	bool IsCurrImageInstanceValid();

	// 当前处理是否存在参数改变
	bool IsCurrProValChanged();

	// 判断当前探测器实例是否存在
	bool IsDetInstanceValid(DetectorNo DetNo);

	// 获取当前显示的图像实例
	void GetCurrImageInstance();

/*********************** 图像显示界面 Start ************************/
// 控件
public:
	QSplitter *pScreenSplitter;   // 屏幕切分容器
	QMdiArea *pScreenMdiArea;     // 用于显示打开的图像

// 参数
public:
	DetectorNo m_DetectorNo;
	DisplayMode m_DisplayMode;

// 函数
public:

	// 显示初始化
	void InitialImgDisplay();

	// 显示模式切换
	void DisplaySwitch();

	// 更新显示图像数据
	void UpdateImage();

	// 选中探测器
	void SeletctDet(DetectorNo DetNo);

// 槽函数
public slots:
	
	// 图像实例更换
	void MdiAreaSwitch(QMdiSubWindow* pSubWidget);
	//图像实例删除
	void DeleteInstance(QString strWinTitle);

/*********************** 图像显示界面 End ************************/

/*********************** 宏处理界面 Start ************************/
public:
	MarcoUI m_MarcoUI;    // 宏处理界面

public:

	// 宏处理界面初始化
	void MarcoUIInit();

	// 更新宏处理管道显示
	void MarcoTabUpdate();

	// 更新处理管道显示更新
	void PipeDisUpdate();

	// 快速处理按钮显示更新
	void FastBtnDisUpdate();

	// 控件使能
	void MarcoEnableCtrl();

// 槽函数
public slots:

	// 宏生成槽函数
	void GenerateMarcoSlot(bool bChecked);

	// 宏加载/编辑槽函数
	void LoadMarcoSlot(bool bChecked);

	// 截取槽函数
	void CutSlot(bool bChecked);

	// 清除槽函数
	void ClearSlot(bool bChecked);

	// 执行宏操作
	void MarcoExcute(QString StrMarcoLabel);

	// 执行宏删除
	void MarcoDelete(QString StrMarcoLabel);

	// 宏生成快速按钮
	void MarcoFastBtn(QString StrMarcoLabel);

/*********************** 宏处理界面 End ************************/


/*********************** 图像处理灰度变换 Start ************************/
public:
	GrayTransUI m_GrayTransUI;	// 灰度变换界面

public slots:

	// 下限滑块值改变
	void SliderLowerValChanged();

	// 中限滑块值改变
	void SliderMidValChanged();

	// 上限滑块值改变
	void SliderUpperValChanged();

	// 下限Spin值变更
	void SpinBottomValChanged(double dInVal);

	// 中限Spin值变更
	void SpinRValChanged(double dInVal);

	// 上限Spin值变更
	void SpinTopValChanged(double dInVal);

	// 应用按钮槽函数
	void GrayTransOK();

	// 撤销按钮槽函数
	void GrayTransCancel();

public:

	// 灰度变换界面初始化
	void GrayTransUIInit();

	// 灰度变换标签页更新
	void GrayTransTabUpdate();

	// 灰度变换界面重置
	void GrayTransTabReset();

	// 使能化控件
	void EnableGrayTransCtrl(bool bEnable);

	// 图像数据加载到直方图
	void ImageDataToHistogram
	(
		QSharedPointer<float> pfData,
		int iWidth,
		int iHeight,
		QSharedPointer<AreaSelect> pArea
	);

	// 将Slider上的中限滑块值转换为实际的R值
	float TransR(int iBottom, int iMid, int iTop);

	// 将实际R值转换为Slider上的中限滑块值
	int VerTransR(int iBottom, int iTop, float fR);

/*********************** 图像处理灰度变换 End ************************/
/*************************** 数据库处理 start ************************/
public:
	QSharedPointer<DataBaseProcess> DBProcess;    //软件中数据库类   
	void DataBaseUIInit();                        //数据库处理界面 


public slots:
	void UpdateDBdisplyInfo(const QModelIndex index); //更新数据库显示信息
	void SearchDataBase();                            //查询数据库
	void UpdateDataBase();                            //更新数据库 
	void DeleteDataBaseEntry();                       //删除记录
	void SubmmitDataBase();                           //提交修改
	void OpenSelectedFile(const QModelIndex index);

	/*************************** 数据库处理 end ************************/


/************************ 图像标记 start *************************/
public:
	//将图元转换为标记信息
	MarksBase*ItemToImageMark(QGraphicsItem*item);
	void GetActWinandCurrentIns();
public slots:
	void MarklistAdd(QGraphicsItem*item);
	void MarklistEdit(QGraphicsItem*item);
	//切换当前标记类型到矩形标记
	void MarkChangeToRect();
	//切换当前标记类型到椭圆标记
	void MarkCHangeToEllipse();
	//切换当前标记类型到自定义形状标记
	void MarkChangeToPolygon();
	//切换当前标记类型到长度测量
	void MarkChangeToRuler();
	//取消鼠标选择类型
	void MarkCancelMouseSelect();
	/******************************** 图像标记 end *******************/


};
