#include "ZCIPSMainFrame.h"
#include "ERRInfo.h"
#include "AddMarcoWidget.h"
#include "LoadMarcoWidget.h"
#include "ImageWidget.h"
/**************************************************************
函数名： ZCIPSMainFrame

定义 ：  构造函数

入口参数 ：
	QWidget *parent ： 父指针

函数返回值 ：

****************************************************************/
ZCIPSMainFrame::ZCIPSMainFrame(QWidget *parent)
	: QMainWindow(parent)
{
	// 参数初始化
	bIsUpdatedDisplay = true;
	bIsUpdateImageIns = true;
	m_DisplayMode = DisplayMode_None;
	
	// 宏类型实例初始化
	pMarco.reset(new Marco);

	// 界面初始化	
	ui.setupUi(this);   // ui初始化
	InitialUI();        // 界面设置于调整

	// 显示设置
	DisplaySwitch();

	// 槽函数设置
	connect(ui.LoadFileAction, &QAction::triggered, this, &ZCIPSMainFrame::LoadImageFromFileSlot);
	connect(ui.MarkRectAction, &QAction::triggered, this, &ZCIPSMainFrame::MarkChangeToRect);
	connect(ui.MarkEllipseAction, &QAction::triggered, this, &ZCIPSMainFrame::MarkCHangeToEllipse);
	connect(ui.MarkPolyAction, &QAction::triggered, this, &ZCIPSMainFrame::MarkChangeToPolygon);
	//connect(ui.CancelSelectAction, &QAction::triggered, this, &ZCIPSMainFrame::MarkCancelMouseSelect);
	connect(ui.MarkRulerAction, &QAction::triggered, this, &ZCIPSMainFrame::MarkChangeToRuler);
}
/**************************************************************
函数名： LoadImageSlot

定义 ：  载入图像按钮响应槽函数

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::LoadImageFromFileSlot()
{

	// 参数声明
	int iRet;									// 函数返回值
	int iHeight;								// 图像高度
	int iWidth;									// 图像宽度
	QSharedPointer<ImageInstance> pImageIns;    // 图像实例
	QString StrFileName;						// 文件路径全名
	QStringList StrlistFileName;				// 文件名list
	QTextCodec* pTCode;							// 字符类型变换指针
	
	// 获取文件对话框
	pTCode = QTextCodec::codecForName("UTF-8");
	StrlistFileName = QFileDialog::getOpenFileNames
		(this, pTCode->toUnicode("打开图像"), 
		 "", tr("Image File(*.tif)"));
	if (true != StrlistFileName.isEmpty())
	{
		// 加载图像
		for (int Loop = 0; Loop < StrlistFileName.size(); Loop++)
		{
			// 图像实例重置
			pImageIns.reset(new ImageInstance);
			
			// 图像实例加载
			StrFileName = StrlistFileName[Loop];
			iRet = pImageIns.get()->LoadFromFile(StrFileName);
			if (SUCCESS != iRet) return;

			// 加载入图像实例列表
			MapImageIns.Insert(StrFileName, pImageIns);
		}
	}
	else return;

	// 显示模式变更
	m_DisplayMode = DisplayMode_ImageList;

	// 图像更新
	DisplaySwitch();

	// Panel更新
	UpdatePanel();

}

/**************************************************************
函数名： MdiAreaSwitch

定义 ：  图像实例更换

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::MdiAreaSwitch(QMdiSubWindow* pSubWidget)
{
	// 参数声明
	int iHeight;
	int iWidth;

	// 空置指针
	Q_UNUSED(pSubWidget);
	
	// 获取当前正在处理的图像实例
	GetCurrImageInstance();
	
	// 更新区域掩码
	pAreaSelect.clear();

	// 更新Panel
	UpdatePanel();

	return;
}

/*****************************************************************/
//图像实例删除
void ZCIPSMainFrame::DeleteInstance(QString strWinTitle)
{
	MapImageIns.Delete(strWinTitle);
}
/**************************************************************
函数名： SwithPanelTab

定义 ：  切换 Panel Tab

入口参数 ：
	int iIndex : Tab编号

函数返回值 ：
	
****************************************************************/
void ZCIPSMainFrame::SwitchPanelTab(int iIndex)
{
	// 参数声明
	int iRet;
	PanelTab m_PanelTab;
	ImageProcessTab m_ImageProcessTab;

	// 将当前图像处理载入图像处理管道
	if (true == bIsUpdateImageIns)
	{
		CurrProccessToPipe();
	}
	
	// 
	m_PanelTab = (PanelTab)iIndex;

	// Panel Tab	
	switch (m_PanelTab)
	{
		// 控制页面 Tab
		case PanelTab_Control:
		{
			
			
			break;
		}
		
		// 图像处理页面 Tab
		case PanelTab_ImagePro:
		{
			// 获取当前的图像处理子页面
			m_ImageProcessTab 
				= (ImageProcessTab)ui.ImageProcessTabWidget->currentIndex();
			
			// ImageProcess Tab
			switch (m_ImageProcessTab)
			{
				// 灰度变换 Tab
				case ImageProcessTab_GrayTrans:
				{
					// 更新灰度变换 Tab
					GrayTransTabUpdate();
					
					// 更新宏处理 Tab
					MarcoTabUpdate();

					break;
				}
				
				// 图像增强 Tab
				case ImageProcessTab_Enhancement:
				{
					
					
					break;
				}
				
				default:
				{
					
					
					break;
				}
				
			}

			break;
		}
		
		default:
		{
			break;
		}
		
	}

	return;
}

/**************************************************************
函数名： SwitchImageProcessTab

定义 ：  切换 ImageProcess Tab

入口参数 ：
	int iIndex : Tab编号

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::SwitchImageProcessTab(int iIndex)
{
	// 参数声明
	int iRet;
	ImageProcessTab m_ImageProcessTab;

	// 将当前图像处理载入图像处理管道
	if (true == bIsUpdateImageIns)
	{
		CurrProccessToPipe();
	}

	// 获取当前图像处理子页面
	m_ImageProcessTab = (ImageProcessTab)iIndex;
	
	// ImageProcess Tab
	switch (m_ImageProcessTab)
	{
		// 灰度变换 Tab
		case ImageProcessTab_GrayTrans:
		{
			// 灰度变换界面更新
			GrayTransTabUpdate();
			
			// 宏界面更新
			MarcoTabUpdate();
			
			break;
		}
		
		// 图像增强 Tab
		case ImageProcessTab_Enhancement:
		{
			// 宏界面更新
			MarcoTabUpdate();
			
			break;
		}

		default:
		{
			// 宏界面更新
			MarcoTabUpdate();

			break;
		}	
	}

	return;
}

/**************************************************************
函数名： MarcoExcute

定义 ：	 宏操作执行槽函数

入口参数 ：
	QString StrMarcoLabel ： 图像宏处理管道标识

函数返回值 ：
	
****************************************************************/
void ZCIPSMainFrame::MarcoExcute(QString StrMarcoLabel)
{
	// 参数声明
	int iRet;
	bool bRet;
	QSharedPointer<ImageProPipe> pProPipe;
	
	// 当前图像实例是否存在判断
	bRet = IsCurrImageInstanceValid();
	if (false == bRet) return;

	// 当前宏处理管道获取
	pProPipe = pMarco.get()->GetImageProPipe(StrMarcoLabel);
	if (true == pProPipe.isNull()) return;

	// 清除当前处理
	PairCurrImageIns.second.get()->ClearCurrProcess();

	// 图像处理管道加载
	iRet = PairCurrImageIns.second.get()->LoadImageProPipe(pProPipe);
	if (SUCCESS != iRet) return;
	
	// 操作界面更新
	UpdatePanel();

	// 图像更新
	UpdateImage();
	
	return;
}

/**************************************************************
函数名： MarcoDelete

定义 ：	 宏删除槽函数

入口参数 ：
	QString StrMarcoLabel ： 图像宏处理管道标识

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::MarcoDelete(QString StrMarcoLabel)
{
	// 删除对应的宏
	pMarco.get()->DeleteMarco(StrMarcoLabel);

	// 需要更新快速处理按钮界面
	FastBtnDisUpdate();
	
	return;
}

/**************************************************************
函数名： MarcoFastBtn

定义 ：	 宏生成快速按钮

入口参数 ：
	QString StrMarcoLabel ： 图像宏处理管道标识

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::MarcoFastBtn(QString StrMarcoLabel)
{
	// 参数声明
	bool bIsBtn;

	// 获取当前宏是否存在快速按钮
	// 然后进行反选处理
	bIsBtn = pMarco.get()->IsFastBtn(StrMarcoLabel);
	if (true == bIsBtn) pMarco.get()->SetFastBtn(StrMarcoLabel, false);
	else pMarco.get()->SetFastBtn(StrMarcoLabel, true);

	// 更新快速处理按钮界面
	FastBtnDisUpdate();
	
	return;
}

/**************************************************************
函数名： SliderLowerValChanged

定义 ：	 灰度变换下限滑块值改变

入口参数 ： 

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::SliderLowerValChanged()
{
	// 参数声明
	int iRet;
	float fBottomVal;
	float fPreBottomVal;
	GrayTransProcess *pGrayTransProcess;    // 因结构限制，无法使用智能指针
											// 特此声明，该类的注销由ProcessPipe完成

	// 获取当前的处理实例
	// 无当前图像处理实例则创建图像处理实例
	// 有当前图像处理实例则获取该处理实例
	if (true != PairCurrImageIns.second.get()->IsCurrProcessValid())
	{
		// 创建灰度变换实例
		pGrayTransProcess = new GrayTransProcess();

		// 将指针赋值给当前处理实例
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.first = ProcessType_GrayTrans;
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.second = (ImageProcess*)pGrayTransProcess;
	}
	else
	{
		pGrayTransProcess = (GrayTransProcess*)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.second;
	}

	// 下限赋值
	fBottomVal = (float)m_GrayTransUI.pTSlider->iGetLowerVal();

	// 判断中限值是否变更
	fPreBottomVal = pGrayTransProcess->GetBottom();
	if (fPreBottomVal == fBottomVal) return;

	// 判断是否进行图像处理与刷新
	if (true == bIsUpdatedDisplay)
	{
		// 下限参数设置
		pGrayTransProcess->SetBottom((float)fBottomVal);
		
		// 图像处理
		iRet = pGrayTransProcess->ImagePro(
			PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
			PairCurrImageIns.second.get()->pImageData.get()->pfProcessingData,
			PairCurrImageIns.second.get()->pImageData.get()->iWidth,
			PairCurrImageIns.second.get()->pImageData.get()->iHeight);
		if (SUCCESS != iRet) return;

		// 图像显示
		UpdateImage();
		
		// 设置为更新无效
		bIsUpdatedDisplay = false;

		// SpinBox数值更改
		m_GrayTransUI.pSpinBottom->setValue(fBottomVal);

		// 设置为更新有效
		bIsUpdatedDisplay = true;
	}
	else
	{
		// SpinBox数值更改
		m_GrayTransUI.pSpinBottom->setValue(fBottomVal);
	}

	return;
}

/**************************************************************
函数名： SliderMidValChanged

定义 ：	 中限滑块值改变

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::SliderMidValChanged()
{
	// 参数声明
	int iRet;
	int iBottom;
	int iMid;
	int iTop;
	float fR;
	float fPreR;
	GrayTransProcess *pGrayTransProcess;    // 因结构限制，无法使用智能指针
											// 特此声明，该类的注销由ProcessPipe完成

	// 获取当前的处理实例
	// 无当前图像处理实例则创建图像处理实例
	// 有当前图像处理实例则获取该处理实例
	if (true != PairCurrImageIns.second.get()->IsCurrProcessValid())
	{
		// 创建灰度变换实例
		pGrayTransProcess = new GrayTransProcess();

		// 将指针赋值给当前处理实例
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.first = ProcessType_GrayTrans;
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.second = (ImageProcess*)pGrayTransProcess;
	}
	else
	{
		pGrayTransProcess = (GrayTransProcess*)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.second;
	}

	// 中限值设置
	iBottom = m_GrayTransUI.pTSlider->iGetLowerVal();
	iMid = m_GrayTransUI.pTSlider->iGetMidVal();
	iTop = m_GrayTransUI.pTSlider->iGetUpperVal();
	fR = TransR(iBottom, iMid, iTop);
	if (-1 == fR) return;

	// 判断下限值是否变更
	fPreR = pGrayTransProcess->GetR();
	if (fR == fPreR) return;

	// 判断是否进行图像处理与刷新
	if (true == bIsUpdatedDisplay)
	{
		// 中限参数设置
		pGrayTransProcess->SetR((float)fR);
		
		// 图像处理
		iRet = pGrayTransProcess->ImagePro(
			PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
			PairCurrImageIns.second.get()->pImageData.get()->pfProcessingData,
			PairCurrImageIns.second.get()->pImageData.get()->iWidth,
			PairCurrImageIns.second.get()->pImageData.get()->iHeight);
		if (SUCCESS != iRet) return;

		// 图像显示
		UpdateImage();

		// 标识位设置
		bIsUpdatedDisplay = false;

		// SpinBox数值更改
		m_GrayTransUI.pSpinR->setValue(fR);

		// 标识位设置
		bIsUpdatedDisplay = true;
	}
	else
	{
		// SpinBox数值更改
		m_GrayTransUI.pSpinR->setValue(fR);
	}

	return;
}

/**************************************************************
函数名： SliderUpperValChanged

定义 ：	 上限滑块值改变

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::SliderUpperValChanged()
{
	// 参数声明
	int iRet;
	float fTopVal;
	float fPreTopVal;
	GrayTransProcess *pGrayTransProcess;    // 因结构限制，无法使用智能指针
											// 特此声明，该类的注销由ProcessPipe完成


	// 获取当前的处理实例
	// 无当前图像处理实例则创建图像处理实例
	// 有当前图像处理实例则获取该处理实例
	if (true != PairCurrImageIns.second.get()->IsCurrProcessValid())
	{
		// 创建灰度变换实例
		pGrayTransProcess = new GrayTransProcess();

		// 将指针赋值给当前处理实例
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.first = ProcessType_GrayTrans;
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.second = (ImageProcess*)pGrayTransProcess;
	}
	else
	{
		pGrayTransProcess = (GrayTransProcess*)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.second;
	}

	// 下限赋值
	fTopVal = (float)m_GrayTransUI.pTSlider->iGetUpperVal();

	// 判断上限值是否变更
	fPreTopVal = pGrayTransProcess->GetTop();
	if (fPreTopVal == fTopVal) return;

	// 判断是否进行图像处理与刷新
	if (true == bIsUpdatedDisplay)
	{
		// 上限参数设置
		pGrayTransProcess->SetTop((float)fTopVal);
		
		// 图像处理
		iRet = pGrayTransProcess->ImagePro(
			PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
			PairCurrImageIns.second.get()->pImageData.get()->pfProcessingData,
			PairCurrImageIns.second.get()->pImageData.get()->iWidth,
			PairCurrImageIns.second.get()->pImageData.get()->iHeight);
		if (SUCCESS != iRet) return;

		// 图像显示
		UpdateImage();

		// 标识位设置
		bIsUpdatedDisplay = false;

		// SpinBox数值更改
		m_GrayTransUI.pSpinTop->setValue(fTopVal);

		// 标识位设置
		bIsUpdatedDisplay = true;
	}
	else
	{
		// SpinBox数值更改
		m_GrayTransUI.pSpinTop->setValue(fTopVal);
	}

	return;
}

/**************************************************************
函数名： SpinBottomValChanged

定义 ：	 下限Spin值改变

入口参数 ：
	double dInVal ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::SpinBottomValChanged(double dInVal)
{
	// 参数声明
	int iRet;
	float fPreBottomVal;
	float fPreTopVal;
	GrayTransProcess *pGrayTransProcess;    // 因结构限制，无法使用智能指针
											// 特此声明，该类的注销由ProcessPipe完成

	// 获取当前的处理实例
	// 无当前图像处理实例则创建图像处理实例
	// 有当前图像处理实例则获取该处理实例
	if (true != PairCurrImageIns.second.get()->IsCurrProcessValid())
	{
		// 创建灰度变换实例
		pGrayTransProcess = new GrayTransProcess();

		// 将指针赋值给当前处理实例
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.first = ProcessType_GrayTrans;
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.second = (ImageProcess*)pGrayTransProcess;
	}
	else
	{
		pGrayTransProcess = (GrayTransProcess*)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.second;
	}

	// 参数赋值
	fPreBottomVal = pGrayTransProcess->GetBottom();
	fPreTopVal = pGrayTransProcess->GetTop();

	// 参数检查
	if ((0 > dInVal) || (dInVal >= (double)fPreTopVal - 10))
	{
		m_GrayTransUI.pSpinBottom->setValue(fPreBottomVal);
		return;
	}

	// Slider参数设置
	m_GrayTransUI.pTSlider->SetLowerVal((int)round(dInVal), true);

	// 若参数相同，则图像不需要改变
	if ((float)dInVal == fPreBottomVal) return;

	// 判断是否需要进行图像更新
	if (true == bIsUpdatedDisplay)
	{
		// 下限参数设置
		pGrayTransProcess->SetBottom((float)dInVal);

		// 图像处理
		iRet = pGrayTransProcess->ImagePro(
			PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
			PairCurrImageIns.second.get()->pImageData.get()->pfProcessingData,
			PairCurrImageIns.second.get()->pImageData.get()->iWidth,
			PairCurrImageIns.second.get()->pImageData.get()->iHeight);
		if (SUCCESS != iRet) return;

		// 图像显示
		UpdateImage();
	}
}

/**************************************************************
函数名： SpinRValChanged

定义 ：	 R值Spin值改变

入口参数 ：
	double dInVal ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::SpinRValChanged(double dInVal)
{
	// 参数声明
	int iRet;
	int iSliderLowerVal;
	int iSliderUpperVal;
	int iSliderMidVal;
	float fPreRVal;
	GrayTransProcess *pGrayTransProcess;    // 因结构限制，无法使用智能指针
											// 特此声明，该类的注销由ProcessPipe完成

	// 获取当前的处理实例
	// 无当前图像处理实例则创建图像处理实例
	// 有当前图像处理实例则获取该处理实例
	if (true != PairCurrImageIns.second.get()->IsCurrProcessValid())
	{
		// 创建灰度变换实例
		pGrayTransProcess = new GrayTransProcess();

		// 将指针赋值给当前处理实例
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.first = ProcessType_GrayTrans;
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.second = (ImageProcess*)pGrayTransProcess;
	}
	else
	{
		pGrayTransProcess = (GrayTransProcess*)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.second;
	}

	// 参数赋值
	fPreRVal = pGrayTransProcess->GetR();

	// 参数检查
	if ((0.1 > dInVal) || (10 < dInVal))
	{
		m_GrayTransUI.pSpinR->setValue(fPreRVal);
		return;
	}

	// Slider参数设置
	iSliderLowerVal = m_GrayTransUI.pTSlider->iGetLowerVal();
	iSliderUpperVal = m_GrayTransUI.pTSlider->iGetUpperVal();
	iSliderMidVal = VerTransR(iSliderLowerVal, iSliderUpperVal, dInVal);
	m_GrayTransUI.pTSlider->SetMidVal(iSliderMidVal, true);

	// 若参数相同，则图像不需要改变
	if ((float)dInVal == fPreRVal) return;

	// 判断是否需要进行图像更新
	if (true == bIsUpdatedDisplay)
	{
		// R值设置
		pGrayTransProcess->SetR((float)dInVal);

		// 图像处理
		iRet = pGrayTransProcess->ImagePro(
			PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
			PairCurrImageIns.second.get()->pImageData.get()->pfProcessingData,
			PairCurrImageIns.second.get()->pImageData.get()->iWidth,
			PairCurrImageIns.second.get()->pImageData.get()->iHeight);
		if (SUCCESS != iRet) return;

		// 图像显示
		UpdateImage();
	}

	return;
}

/**************************************************************
函数名： SpinTopValChanged

定义 ：	 上限Spin值改变

入口参数 ：
	double dInVal ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::SpinTopValChanged(double dInVal)
{
	// 参数声明
	int iRet;
	float fPreBottomVal;
	float fPreTopVal;
	GrayTransProcess *pGrayTransProcess;    // 因结构限制，无法使用智能指针
											// 特此声明，该类的注销由ProcessPipe完成

	// 获取当前的处理实例
	// 无当前图像处理实例则创建图像处理实例
	// 有当前图像处理实例则获取该处理实例
	if (true != PairCurrImageIns.second.get()->IsCurrProcessValid())
	{
		// 创建灰度变换实例
		pGrayTransProcess = new GrayTransProcess();

		// 将指针赋值给当前处理实例
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.first = ProcessType_GrayTrans;
		PairCurrImageIns.second.get()->pImageProcessIns.get()
			->PairCurrProcess.second = (ImageProcess*)pGrayTransProcess;
	}
	else
	{
		pGrayTransProcess = (GrayTransProcess*)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.second;
	}

	// 参数赋值
	fPreBottomVal = pGrayTransProcess->GetBottom();
	fPreTopVal = pGrayTransProcess->GetTop();

	// 参数检查
	if ((255 < dInVal) || (dInVal <= (double)fPreBottomVal + 10))
	{
		m_GrayTransUI.pSpinTop->setValue(fPreTopVal);
		return;
	}

	// Slider参数设置
	m_GrayTransUI.pTSlider->SetUpperVal((int)round(dInVal), true);

	// 若参数相同，则图像不需要改变
	if ((float)dInVal == fPreTopVal) return;

	// 判断是否需要进行图像更新
	if (true == bIsUpdatedDisplay)
	{
		// 下限参数设置
		pGrayTransProcess->SetTop((float)dInVal);

		// 图像处理
		iRet = pGrayTransProcess->ImagePro(
			PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
			PairCurrImageIns.second.get()->pImageData.get()->pfProcessingData,
			PairCurrImageIns.second.get()->pImageData.get()->iWidth,
			PairCurrImageIns.second.get()->pImageData.get()->iHeight);
		if (SUCCESS != iRet) return;

		// 图像显示
		UpdateImage();
	}

	return;
}

/**************************************************************
函数名： GrayTransOK

定义 ：	 应用按钮槽函数

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::GrayTransOK()
{
	// 参数声明
	int iRet;
	bool bRet;
	
	// 当前图像实例检查
	bRet = IsCurrImageInstanceValid();
	if (false == bRet) return;
	
	// 当前处理检查
	bRet = PairCurrImageIns.second.get()->IsCurrProcessValid();
	if (false == bRet) return;

	// 将当前处理实例加入处理管道
	CurrProccessToPipe();

	// 更新Panel界面
	UpdatePanel();

	return;
}

/**************************************************************
函数名： GrayTransCancel

定义 ：	 撤销按钮槽函数

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::GrayTransCancel()
{
	// 参数声明
	int iRet;
	bool bRet;

	// 当前图像实例检查
	bRet = IsCurrImageInstanceValid();
	if (false == bRet) return;

	// 当前处理检查
	bRet = PairCurrImageIns.second.get()->IsCurrProcessValid();
	if (false == bRet) return;

	// 清除当前图像处理
	PairCurrImageIns.second.get()
		->pImageProcessIns.get()->ClearCurrProcess();

	// 当前处理中的图像被撤销
	iRet = PairCurrImageIns.second.get()->pImageData.get()->ProcessingCancel();
	if (SUCCESS != iRet) return;

	// 更新图像显示
	UpdateImage();

	// 更新Panel界面
	UpdatePanel();

	return;
}

/**************************************************************
函数名： UpdateImage

定义 ：  更新显示图像数据
         依据当前图像实例的数据，对图像显示数据进行更新

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::UpdateImage()
{
	// 参数声明
	bool bRet;							// 返回值
	int iRet;                           // 返回值
	int iMdiHeight;						// Mdi高度
	int iMdiWidth;						// Mdi宽度
	QString StrLabel;					// 标识
	ImageWidget *pSubWidget;				// 子窗口指针
	//QWidget *pSubWidget;
	// 参数赋值
	pSubWidget = nullptr;

	// 判断当前图像实例是否存在
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;

	// 标识获取
	StrLabel = PairCurrImageIns.first;

	// 获取当前窗口子Widget指针
	switch (m_DisplayMode)
	{
		// 探测器显示
		case DisplayMode_Detector:
		{
			switch (m_DetectorNo)
			{
				// 当前处理为1号探测器
				case Detector_1:
				{
					if (nullptr != pScreenSplitter)
					{
						pSubWidget = (ImageWidget*)pScreenSplitter->widget(0);
					}
				
					break;
				}
				
				// 当前处理为2号探测器
				case Detector_2:
				{
					if (nullptr != pScreenSplitter)
					{
						pSubWidget = (ImageWidget*)pScreenSplitter->widget(1);
					}
					
					break;
				}
				
				default:
				{
					break;
				}
				
			}

			break;
		}
		
		// 图像列表显示
		case DisplayMode_ImageList:
		{
			// 获取当前激活的子窗口
			pSubWidget =(ImageWidget*) pScreenMdiArea->currentSubWindow();

			// 判断激活窗口与图像实例是否匹配
			// 若不匹配，则取消处理
			if (StrLabel != pSubWidget->windowTitle())
			{
				return;
			}

			break;
		}

		case DisplayMode_None:
		default:
		{
			break;
		}
	}

	// 初始化
	pImageDisplay.reset(new ImageDisplay());

	// 判断是否获得子窗口
	// 对应子窗口不存在，则退出
	if (nullptr == pSubWidget) return;
	
	// 将图像实例转换为图像
	iRet = pImageDisplay.get()->InstanceToDisplay(
		PairCurrImageIns.second,
		pSubWidget);
	if (SUCCESS != iRet) return;

	return;
}

/**************************************************************
函数名： SeletctDet

定义 ：  选择探测器

入口参数 ：
	DetectorNo DetNo ： 探测器编号

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::SeletctDet(DetectorNo DetNo)
{
	switch (DetNo)
	{
		case Detector_1:
		{
			// 当前选择设置
			m_DetectorNo = Detector_1;
			
			// 边框设置
			pScreenSplitter->widget(0)
				->setStyleSheet("background-color:white;border: 2px solid #33CCFF;");
			pScreenSplitter->widget(1)
				->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");

			break;
		}

		case Detector_2:
		{
			// 当前选择设置
			m_DetectorNo = Detector_2;

			// 边框设置
			pScreenSplitter->widget(1)
				->setStyleSheet("background-color:white;border: 2px solid #33CCFF;");
			pScreenSplitter->widget(0)
				->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");
	
			break;
		}
		
		default:
		{
			break;
		}
	}

	return;
}

/**************************************************************
函数名： CurrProccessToPipe

定义 ：  将当前处理加入图像处理管道

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::CurrProccessToPipe()
{
	// 参数声明
	bool bRet;
	int iRet;

	// 判断当前图像实例是否存在
	bRet = IsCurrImageInstanceValid();
	if (false == bRet) return;

	// 判断当前处理是否有有效的参数改变
	bRet = IsCurrProValChanged();
	if (false == bRet) return;

	// 将当前图像处理载入图像处理管道
	iRet = PairCurrImageIns.second.get()->AddProcessPipe();
	if (SUCCESS != iRet) return;

	// pfCurrData -> pfProccessingData
	iRet = PairCurrImageIns.second.get()->DataProccessAccept();
	if (SUCCESS != iRet) return;

	return;
}
/**************************************************************
函数名： InitialUI

定义 ：  初始化UI设置

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::InitialUI()
{
	// 参数声明
	QRect RectScreen;
	int iHeight;

	// 指针赋值

	// UI设置
	this->setWindowState(Qt::WindowMaximized);
	this->setWindowFlags(this->windowFlags()
		&~Qt::WindowMaximizeButtonHint
		&~Qt::WindowMinMaxButtonsHint);
	RectScreen = QApplication::desktop()->availableGeometry();
	this->setFixedHeight(RectScreen.height()- 20);
	this->setFixedWidth(RectScreen.width());
	ui.PanelDockWidget->setFixedWidth(400);
	ui.PanelDockWidget->setFixedHeight(this->height() - 20);
	ui.PanelTabWidget->setFixedWidth(395);
	ui.PanelTabWidget->setFixedHeight(ui.PanelDockWidget->height() - 40);
	ui.ImageProcessTabWidget->setFixedWidth(390);
	ui.ImageProcessTabWidget->setFixedHeight(ui.PanelTabWidget->height() - 30 - 300);
	ui.MarcoBox->setGeometry(ui.ImageProcessTabWidget->x(), 
		ui.ImageProcessTabWidget->y() + ui.ImageProcessTabWidget->height() + 5,
		388, 300);
	ui.GrayTrans->setGeometry(0, 0, ui.PanelTabWidget->width() - 10, ui.PanelTabWidget->height());
	ui.Enhancement->setGeometry(0, 0, ui.PanelTabWidget->width() - 10, ui.PanelTabWidget->height());

	// 灰度变换界面初始化
	GrayTransUIInit();

	// 宏处理界面初始化
	MarcoUIInit();

	// 图像显示初始化
	InitialImgDisplay();

	// 显示界面设置
	ui.centralWidget->setMouseTracking(true);

	// 建立槽函数与信号的连接 
	connect(ui.PanelTabWidget, SIGNAL(currentChanged(int)), this, SLOT(SwitchPanelTab(int)));
	connect(ui.ImageProcessTabWidget, SIGNAL(currentChanged(int)), this, SLOT(SwitchImageProcessTab(int)));
	//数据库界面
	DataBaseUIInit();

	ctScanHandle = CTScanInterface::loadCtScanInstance(ui.DeviceControlTab);
	connect(ui.LoadFileAction, &QAction::triggered, ctScanHandle, &CTScanInterface::slot1);
	connect(ctScanHandle, &CTScanInterface::signal1, this, &ZCIPSMainFrame::ClearSlot);
}

/**************************************************************
函数名： MarcoUIInit

定义 ：  宏处理初始化UI设置

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::MarcoUIInit()
{
	// 初始化
	m_MarcoUI.pMarcoPipeArea = new MarcoPipeArea(ui.MarcoBox);
	m_MarcoUI.pFastBtnArea = new FastBtnArea(ui.MarcoBox);
	m_MarcoUI.pBtnMarcoGenerate = ui.ButtonMarcoGenerate;
	m_MarcoUI.pBtnMarcoLoad = ui.ButtonMarcoLoad;
	m_MarcoUI.pBtnMarcoClear = ui.ButtonMarcoClear;
	m_MarcoUI.pBtnPipeCut = ui.ButtonPipleCut;
	m_MarcoUI.pBtnFastEdit = ui.ButtonFastEdit;

	// 几何位置设置 
	m_MarcoUI.pBtnMarcoLoad->setGeometry(10, 25, 84, 25);
	m_MarcoUI.pBtnMarcoGenerate->setGeometry(
		10 + m_MarcoUI.pBtnMarcoLoad->width() + 10, 25, 84, 25);
	m_MarcoUI.pBtnPipeCut->setGeometry(
		10 + m_MarcoUI.pBtnMarcoLoad->width() 
		+ 10 + m_MarcoUI.pBtnMarcoGenerate->width() + 10,
		25, 84, 25);
	m_MarcoUI.pBtnMarcoClear->setGeometry(
		10 + m_MarcoUI.pBtnMarcoLoad->width() 
		+ 10 + m_MarcoUI.pBtnMarcoGenerate->width() 
		+ 10 + m_MarcoUI.pBtnPipeCut->width() + 10,
		25, 84, 25);
	m_MarcoUI.pMarcoPipeArea->setGeometry(10,
		25 + m_MarcoUI.pBtnMarcoLoad->height() + 10,
		ui.MarcoBox->width() - 20, 130);
	m_MarcoUI.pMarcoPipeArea->widget()->setGeometry(0, 0,
		m_MarcoUI.pMarcoPipeArea->width(),
		m_MarcoUI.pMarcoPipeArea->height() - 20);
	m_MarcoUI.pFastBtnArea->setGeometry(10,
		25 + m_MarcoUI.pBtnMarcoLoad->height()
		+ 10 + m_MarcoUI.pMarcoPipeArea->height() + 20,
		m_MarcoUI.pMarcoPipeArea->width() - 80, 80);
	m_MarcoUI.pBtnFastEdit->setGeometry(
		10 + m_MarcoUI.pFastBtnArea->width(),
		25 + m_MarcoUI.pBtnMarcoLoad->height()
		+ 10 + m_MarcoUI.pMarcoPipeArea->height() + 20,
		(m_MarcoUI.pMarcoPipeArea->width() - m_MarcoUI.pFastBtnArea->width()), 80);
	
	// 快速按钮界面更新
	FastBtnDisUpdate();

	// 控件使能
	MarcoEnableCtrl();

	// 槽函数与信号链接
	connect(m_MarcoUI.pBtnMarcoGenerate, SIGNAL(clicked(bool)),
		this, SLOT(GenerateMarcoSlot(bool)));
	connect(m_MarcoUI.pBtnMarcoLoad, SIGNAL(clicked(bool)),
		this, SLOT(LoadMarcoSlot(bool)));
	connect(m_MarcoUI.pBtnFastEdit, SIGNAL(clicked(bool)),
		this, SLOT(LoadMarcoSlot(bool)));
	connect(m_MarcoUI.pBtnPipeCut, SIGNAL(clicked(bool)),
		this, SLOT(CutSlot(bool)));
	connect(m_MarcoUI.pBtnMarcoClear, SIGNAL(clicked(bool)),
		this, SLOT(ClearSlot(bool)));

	return;
}

/**************************************************************
函数名： UpdateMarcoPiple

定义 ：  更新宏处理管道显示

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::MarcoTabUpdate()
{
	// 处理管道显示更新
	PipeDisUpdate();

	// 快速处理按钮显示更新
	FastBtnDisUpdate();

	// 控件使能
	MarcoEnableCtrl();
	
}

/**************************************************************
函数名： PipeDisUpdate

定义 ：  更新处理管道显示

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::PipeDisUpdate()
{
	// 参数声明
	int iImgProcessSize;
	int iRet;
	bool bRet;
	QString StrPara;
	QString StrProcessName;
	ProcessType InType;
	ImageProcess *pImageProcess;
	QTextCodec *pTCode;

	// 判断当前图像处理实例是否存在
	bRet = IsCurrImageInstanceValid();
	if (false == bRet)
	{
		// 重置当前的宏处理管道显示
		m_MarcoUI.pMarcoPipeArea->ClearPipe();

		return;
	}

	// 重置当前的宏处理管道显示
	m_MarcoUI.pMarcoPipeArea->ClearPipe();

	// 获取当前宏处理管道的长度
	iImgProcessSize = PairCurrImageIns.second.get()->pImageProcessIns.get()
		->pProcessPipe.get()->GetPipeLen();

	// 加载入宏处理通道
	for (int i = 0; i < iImgProcessSize; i++)
	{
		// 获取处理类型与参数
		iRet = PairCurrImageIns.second.get()
			->pImageProcessIns.get()
			->pProcessPipe.get()->PipeGet
			(i, InType, &pImageProcess);
		if (SUCCESS != iRet) return;

		// 获取参数字符
		switch (InType)
		{
			case ProcessType_GrayTrans:
			{
				// 获取参数
				iRet = pImageProcess->GetParaString(StrPara);
				if (SUCCESS != iRet) return;

				// 处理步骤名
				pTCode = QTextCodec::codecForName("GBK");
				StrProcessName = pTCode->toUnicode("灰度变换");

				break;
			}

			case ProcessType_Enhancement:
			{
				continue;
			}

			case ProcessType_None:
			default:
			{
				continue;
			}
		}

		// 加载进入ScrollArea控件
		m_MarcoUI.pMarcoPipeArea->AddPipeUnit(StrProcessName, StrPara);
	}

	// 选择
	m_MarcoUI.pMarcoPipeArea->SelectWidget();

	return;
}

/**************************************************************
函数名： FastBtnDisUpdate

定义 ：  快速处理按钮显示更新

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::FastBtnDisUpdate()
{
	// 参数声明
	QVector<FastButton *>::iterator it;
	QVector<FastButton *> vecFastBtn;
	
	// 更新区域选择
	m_MarcoUI.pFastBtnArea->FastBtnUpdate(pMarco);

	// 建立消息与槽函数的链接
	vecFastBtn = m_MarcoUI.pFastBtnArea->GetBtnVec();
	for (it = vecFastBtn.begin(); it != vecFastBtn.end(); it++)
	{
		connect(*it, SIGNAL(FastBtn(QString)), this, SLOT(MarcoExcute(QString)));
	}
	
	return;
}

/**************************************************************
函数名： EnableCtrl

定义 ：  控件使能

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::MarcoEnableCtrl()
{
	// 是否有正在处理图像实例
	if (true == IsCurrImageInstanceValid())
	{
		// 是否存在处理管道
		if (true == PairCurrImageIns.second.get()->IsProcessPipeValid())
		{
			m_MarcoUI.pBtnMarcoGenerate->setEnabled(true);
			m_MarcoUI.pBtnMarcoLoad->setEnabled(true);
			m_MarcoUI.pBtnPipeCut->setEnabled(true);
			m_MarcoUI.pBtnMarcoClear->setEnabled(true);
			m_MarcoUI.pBtnFastEdit->setEnabled(true);
			m_MarcoUI.pMarcoPipeArea->setEnabled(true);
			m_MarcoUI.pFastBtnArea->setEnabled(true);
		}
		else
		{
			m_MarcoUI.pBtnMarcoGenerate->setEnabled(false);
			m_MarcoUI.pBtnMarcoLoad->setEnabled(true);
			m_MarcoUI.pBtnPipeCut->setEnabled(false);
			m_MarcoUI.pBtnMarcoClear->setEnabled(false);
			m_MarcoUI.pBtnFastEdit->setEnabled(true);
			m_MarcoUI.pMarcoPipeArea->setEnabled(true);
			m_MarcoUI.pFastBtnArea->setEnabled(true);
		}
	}
	else
	{
		m_MarcoUI.pBtnMarcoGenerate->setEnabled(false);
		m_MarcoUI.pBtnMarcoLoad->setEnabled(true);
		m_MarcoUI.pBtnPipeCut->setEnabled(false);
		m_MarcoUI.pBtnMarcoClear->setEnabled(false);
		m_MarcoUI.pBtnFastEdit->setEnabled(true);
		m_MarcoUI.pMarcoPipeArea->setEnabled(false);
		m_MarcoUI.pFastBtnArea->setEnabled(false);
	}
}

 
/**************************************************************
函数名： GenerateMarcoSlot

定义 ：  宏生成槽函数

入口参数 ：
	bool bChecked ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::GenerateMarcoSlot(bool bChecked)
{
	// 参数声明 
	bool bRet;
	AddMarcoWidget *pGenWidget;

	// 判断当前图像实例是否存在
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;

	// 初始化
	pGenWidget = new AddMarcoWidget(
		pMarco, 
		PairCurrImageIns.second,
		nullptr);

	// 打开对话框
	pGenWidget->show();

	return;
}

/**************************************************************
函数名： GenerateMarcoSlot

定义 ：  宏加载/编辑槽函数

入口参数 ：
	bool bChecked ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::LoadMarcoSlot(bool bChecked)
{
	// 参数声明 
	LoadMarcoWidget *pLoadMarcoWidget;
	
	// 初始化
	pLoadMarcoWidget = new LoadMarcoWidget(
		PairCurrImageIns.second,
		pMarco,
		nullptr);

	// 槽函数与信号链接
	connect(pLoadMarcoWidget, SIGNAL(ExcuteEmit(QString)),
		this, SLOT(MarcoExcute(QString)));
	connect(pLoadMarcoWidget, SIGNAL(DeleteEmit(QString)),
		this, SLOT(MarcoDelete(QString)));
	connect(pLoadMarcoWidget, SIGNAL(FastBtnEmit(QString)),
		this, SLOT(MarcoFastBtn(QString)));

	// 打开对话框
	pLoadMarcoWidget->show();
}

/**************************************************************
函数名： CutSlot

定义 ：  截取槽函数

入口参数 ：
	bool bChecked ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::CutSlot(bool bChecked)
{
	// 参数声明
	int iRet;
	bool bRet;         // 函数返回值
	int iPipeSize;
	int iSelProNo; 
	
	// 判断当前图像实例是否存在
	bRet = IsCurrImageInstanceValid();
	if (false == bRet) return;

	// 判断处理管道是否存在
	bRet = PairCurrImageIns.second.get()->IsProcessPipeValid();
	if (false == bRet) return;

	// 获取当前处理管道长度
	iPipeSize = PairCurrImageIns.second.get()->ProPipeSize();
	if (0 >= iPipeSize) return;

	// 获取当前选择的处理步骤编号
	// 并进行判断
	iSelProNo = m_MarcoUI.pMarcoPipeArea->GetCurrSel();
	if ((0 > iSelProNo) || (iPipeSize <= iSelProNo)) return;
	
	// 清除当前处理
	PairCurrImageIns.second.get()->ClearCurrProcess();

	// 管道截取
	iRet = PairCurrImageIns.second.get()->ProPipeCut(iSelProNo);
	if (SUCCESS != iRet) return;

	// 执行新的图像处理管道
	// 进行图像处理
	iRet = PairCurrImageIns.second.get()->ProPipeExcute();
	if (SUCCESS != iRet) return;

	// 操作界面更新
	UpdatePanel();

	// 图像更新
	UpdateImage();
	
	return;
}

/**************************************************************
函数名： ClearSlot

定义 ：  清除槽函数

入口参数 ：
	bool bChecked ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::ClearSlot(bool bChecked)
{	
	// 清除当前处理
	PairCurrImageIns.second.get()->ClearCurrProcess();

	// 清除管道处理
	PairCurrImageIns.second.get()->ClearProPipe();

	// 数据重置
	PairCurrImageIns.second.get()->ResetImgData();

	// 操作界面更新
	UpdatePanel();

	// 图像更新
	UpdateImage();

	return;
}


/**************************************************************
函数名： GrayTranUIInit

定义 ：  灰度变换初始化UI设置

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::GrayTransUIInit()
{
	// 初始化
	m_GrayTransUI.pSpinBottom = ui.SpinBottomVal;
	m_GrayTransUI.pSpinR = ui.SpinRVal;
	m_GrayTransUI.pSpinTop = ui.SpinTopVal;
	m_GrayTransUI.pButtonCancel = ui.ButtonCancel;
	m_GrayTransUI.pButtonOK = ui.ButtonOK;
	
	m_GrayTransUI.pChartGray = new QChart();
	m_GrayTransUI.pChartViewGray = new QChartView(m_GrayTransUI.pChartGray, ui.GrayTrans);
	m_GrayTransUI.pTSlider = new TripleSlider(Qt::Horizontal, ui.GrayTrans);

	// 几何位置设置
	m_GrayTransUI.pChartViewGray->setGeometry(
		(ui.GrayTrans->width() - 350) / 2, 0, 350, 300);
	m_GrayTransUI.pTSlider->setGeometry(
		(ui.GrayTrans->width() - 340) / 2, m_GrayTransUI.pChartViewGray->height(), 340, 20);
	m_GrayTransUI.pSpinBottom->setGeometry(
		(ui.GrayTrans->width() - 350) / 2,
		m_GrayTransUI.pChartViewGray->height() + m_GrayTransUI.pTSlider->height() + 10,
		60, 20);
	m_GrayTransUI.pSpinR->setGeometry(
		(ui.GrayTrans->width() / 2) - 30, 
		m_GrayTransUI.pChartViewGray->height() + m_GrayTransUI.pTSlider->height() + 10,
		60, 20);
	m_GrayTransUI.pSpinTop->setGeometry(
		ui.GrayTrans->width() - ((ui.GrayTrans->width() - 350) / 2) - 60,
		m_GrayTransUI.pChartViewGray->height() + m_GrayTransUI.pTSlider->height() + 10,
		60, 20);
	m_GrayTransUI.pButtonCancel->setGeometry(
		(ui.GrayTrans->width() - 350) / 2,
		m_GrayTransUI.pChartViewGray->height() 
		+ m_GrayTransUI.pTSlider->height() + 10 
		+ m_GrayTransUI.pSpinBottom->height() + 20,
		80, 30);
	m_GrayTransUI.pButtonOK->setGeometry(
		(ui.GrayTrans->width() - 350) / 2 + 10
		+ m_GrayTransUI.pButtonCancel->width(),
		m_GrayTransUI.pChartViewGray->height()
		+ m_GrayTransUI.pTSlider->height() + 10
		+ m_GrayTransUI.pSpinBottom->height() + 20,
		80, 30);

	// 设置控件可用性
	EnableGrayTransCtrl(false);

	// 显示
	m_GrayTransUI.pTSlider->show();

	// Spinbox设置
	m_GrayTransUI.pSpinBottom->setRange(0, 245);
	m_GrayTransUI.pSpinBottom->setValue(0);
	m_GrayTransUI.pSpinBottom->setSingleStep(1);
	m_GrayTransUI.pSpinTop->setRange(10, 255);
	m_GrayTransUI.pSpinTop->setValue(255);
	m_GrayTransUI.pSpinTop->setSingleStep(1);
	m_GrayTransUI.pSpinR->setRange(0.1, 10);
	m_GrayTransUI.pSpinR->setValue(1);
	m_GrayTransUI.pSpinR->setSingleStep(0.1);
	m_GrayTransUI.pSpinR->setDecimals(4);

	// 建立槽函数与信号的连接
	connect(m_GrayTransUI.pTSlider, SIGNAL(LowerValueChanged()), this, SLOT(SliderLowerValChanged()));
	connect(m_GrayTransUI.pTSlider, SIGNAL(MidValueChanged()), this, SLOT(SliderMidValChanged()));
	connect(m_GrayTransUI.pTSlider, SIGNAL(UpperValueChanged()), this, SLOT(SliderUpperValChanged()));
	connect(m_GrayTransUI.pSpinBottom, SIGNAL(valueChanged(double)), this, SLOT(SpinBottomValChanged(double)));
	connect(m_GrayTransUI.pSpinTop, SIGNAL(valueChanged(double)), this, SLOT(SpinTopValChanged(double)));
	connect(m_GrayTransUI.pSpinR, SIGNAL(valueChanged(double)), this, SLOT(SpinRValChanged(double)));
	connect(m_GrayTransUI.pButtonOK, SIGNAL(clicked()), this, SLOT(GrayTransOK()));
	connect(m_GrayTransUI.pButtonCancel, SIGNAL(clicked()), this, SLOT(GrayTransCancel()));

}

/**************************************************************
函数名： GrayTransTabUpdate

定义 ：  

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::GrayTransTabUpdate()
{
	// 函数返回值
	bool bRet;
	ProcessType m_PType;
	GrayTransProcess *pGranTransProcess;

	// 灰度变换界面重置
	GrayTransTabReset();
	
	// 判断当前图像实例是否存在
	// 如果不存在，则返回
	bRet = IsCurrImageInstanceValid();
	if (false == bRet) return;

	// 载入图像数据至直方图
	ImageDataToHistogram(
		PairCurrImageIns.second.get()->pImageData.get()->pfCurrData,
		PairCurrImageIns.second.get()->pImageData.get()->iWidth,
		PairCurrImageIns.second.get()->pImageData.get()->iHeight,
		pAreaSelect);

	// 按钮使能
	EnableGrayTransCtrl(true);

	// 判断当前图像处理是否存在
	// 并依据不同情况进行不同处理
	bRet = PairCurrImageIns.second.get()->IsCurrProcessValid();
	m_PType = PairCurrImageIns.second.get()
		->pImageProcessIns.get()->PairCurrProcess.first;
	if ((true == bRet) && (ProcessType_GrayTrans == m_PType))
	{
		// 获取当前图像处理指针
		pGranTransProcess = 
			(GrayTransProcess *)PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess                                                                                                          .second;
		
		bIsUpdatedDisplay = false;

		// 参数修改
		m_GrayTransUI.pSpinBottom->setValue(
			(double)pGranTransProcess->GetBottom());
		m_GrayTransUI.pSpinTop->setValue(
			(double)pGranTransProcess->GetTop());
		m_GrayTransUI.pSpinR->setValue(
			(double)pGranTransProcess->GetR());

		bIsUpdatedDisplay = true;
	}
	return;
}

/**************************************************************
函数名： EnableGrayTransCtrl

定义 ：  使能化控件
	
入口参数 ：
	bool bEnable :		使能标识

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::EnableGrayTransCtrl(bool bEnable)
{
	if (true == bEnable)
	{
		m_GrayTransUI.pSpinBottom->setEnabled(true);
		m_GrayTransUI.pSpinR->setEnabled(true);
		m_GrayTransUI.pSpinTop->setEnabled(true);
		m_GrayTransUI.pTSlider->setEnabled(true);
		m_GrayTransUI.pButtonCancel->setEnabled(true);
		m_GrayTransUI.pButtonOK->setEnabled(true);
	}
	else
	{
		m_GrayTransUI.pSpinBottom->setEnabled(false);
		m_GrayTransUI.pSpinR->setEnabled(false);
		m_GrayTransUI.pSpinTop->setEnabled(false);
		m_GrayTransUI.pTSlider->setEnabled(false);
		m_GrayTransUI.pButtonCancel->setEnabled(false);
		m_GrayTransUI.pButtonOK->setEnabled(false);
	}
}

/**************************************************************
函数名： ImageDataToHistogram

定义 ：  图像数据加载到直方图

入口参数 ：
	QSharedPointer<float> pfData ：
	int iWidth ：
	int iHeight ：
	QSharedPointer<AreaSelect> pArea ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::ImageDataToHistogram
(
	QSharedPointer<float> pfData,
	int iWidth,
	int iHeight,
	QSharedPointer<AreaSelect> pArea
)
{
	// 参数声明
	int iGrayVal;
	float afHistogram[256];
	float fMax;
	QLineSeries *pLineGray;
	QAreaSeries *pAreaGray;
	QSharedPointer<UCHAR> pucAreaMask;

	// 区域判断
	if (true == pArea.isNull())
	{
		pucAreaMask.reset(new UCHAR[iHeight * iWidth]);
		memset(pucAreaMask.get(), 1, iHeight * iWidth * sizeof(UCHAR));
	}
	else
	{
		pucAreaMask = pArea.get()->pucAreaMask;
	}

	// 灰度分布统计
	memset(afHistogram, 0, 255 * sizeof(float));
	for (int i = 0; i < iHeight; i++)
		for (int j = 0; j < iWidth; j++)
		{
			if ((UCHAR)1 == pucAreaMask.get()[i * iWidth + j])
			{
				// 灰度值取整
				iGrayVal = (int)round(pfData.get()[i * iWidth + j]);

				// 考虑灰度分布图的可看性
				// [0]与[255]不予统计
				if ((0 == iGrayVal) || (255 == iGrayVal)) continue;

				// 统计值赋值
				afHistogram[iGrayVal] += (float)1;
			}
		}

	// 灰度值归一化到 0 - 1
	fMax = afHistogram[0];
	for (int i = 1; i < 256; i++)
	{
		fMax = qMax(fMax, afHistogram[i]);
	}
	for (int i = 0; i < 256; i++)
	{
		afHistogram[i] = afHistogram[i] / fMax;
	}

	// 直方图图像重置
	m_GrayTransUI.pChartGray->removeAllSeries();

	// 灰度直方图绘制
	pLineGray = new QLineSeries(m_GrayTransUI.pChartGray);
	for (int i = 0; i < 256; i++)
	{
		pLineGray->append(QPointF(i, afHistogram[i]));
	}
	pAreaGray = new QAreaSeries(pLineGray, nullptr);
	m_GrayTransUI.pChartGray->addSeries(pAreaGray);

	// Chart设置
	m_GrayTransUI.pChartGray->createDefaultAxes();
	m_GrayTransUI.pChartGray->axisX()->setRange(0, 255);
	m_GrayTransUI.pChartGray->axisY()->setRange(0, 1);
	m_GrayTransUI.pChartGray->removeAxis(m_GrayTransUI.pChartGray->axisY());
	m_GrayTransUI.pChartGray->removeAxis(m_GrayTransUI.pChartGray->axisX());

	// 边缘设置
	m_GrayTransUI.pChartGray->setMargins(QMargins(0, 0, 0, 0));
	
	return;
}

/**************************************************************
函数名： TransR

定义 ：  将控件上的中限滑块位置转换为实际的R值

入口参数 ：
	int iBottom ：
	int iMid ：
	int iTop ：

函数返回值 ：
	float fR:  错误 -- -1
			   正确 -- 0.1 - 10
****************************************************************/
float ZCIPSMainFrame::TransR
(
	int iBottom,
	int iMid,
	int iTop
)
{
	// 参数声明
	int iRange;
	int iHandlePos;
	float fR;
	float fInterval;

	// 参数检查
	if ((iTop < iBottom + 10) || (iMid <= iBottom) || (iMid >= iTop))
	{
		return -1;
	}

	// 参数赋值
	iRange = iTop - iBottom - 1;
	iHandlePos = iMid - iBottom - 1;

	// 滑块驻留点数量为偶数，中间两个点对应 R = 1
	// 滑块驻留点数量为奇数，中间单个点对应 R = 1
	if (0 == (iRange % 2))		// 滑块驻留点数量为偶数
	{
		// 间距计算
		fInterval = (float)1 / (float)((iRange / 2) - 1);

		// 计算R值
		if ((float)iHandlePos < ((float)(iRange - 1) / (float)2))   // 上半区
		{
			fR = pow(10, (float)(iHandlePos - (iRange / 2 - 1)) * fInterval);
		}
		else                                                        // 下半区
		{
			fR = pow(10, (float)(iHandlePos - (iRange / 2)) * fInterval);
		}

	}
	else  // 滑块驻留点数量为奇数
	{
		// 间距计算
		fInterval = (float)1 / (float)((iRange - 1) / 2);

		// 计算R值
		fR = pow(10, (float)(iHandlePos - ((iRange - 1) / 2)) * fInterval);	
	}

	return fR;
}

/**************************************************************
函数名： VerTransR

定义 ：  将控件上的中限滑块位置转换为实际的R值

入口参数 ：
	int iBottom ：
	int iTop ：
	float fR ：

函数返回值 ：
	int iMid:  错误 -- -1
****************************************************************/
int ZCIPSMainFrame::VerTransR
(
	int iBottom,
	int iTop,
	float fR
)
{
	// 参数声明
	int iMid;
	int iRange;
	float fInterval;
	float fLogR;

	// 参数检查
	if (iTop < iBottom + 10) return -1;

	// 参数赋值
	iRange = iTop - iBottom - 1;
	fLogR = log10(fR);

	// 滑块驻留点数量为偶数，中间两个点对应 R = 1
	// 滑块驻留点数量为奇数，中间单个点对应 R = 1
	if (0 == (iRange % 2))		// 滑块驻留点数量为偶数
	{
		// 间距计算
		fInterval = (float)1 / (float)((iRange / 2) - 1);

		// 计算Mid值
		if ((fLogR / fInterval) <= 0)
		{
			iMid = (iRange / 2) + (int)round(fLogR / fInterval);
		}
		else
		{
			iMid = (iRange / 2 + 1) + (int)round(fLogR / fInterval);
		}
	}
	else  // 滑块驻留点数量为奇数
	{
		// 间距计算
		fInterval = (float)1 / (float)((iRange - 1) / 2);

		// 计算Mid值
		iMid = (iRange + 1) / 2 + (int)round(fLogR / fInterval);
	}

	return iBottom + iMid;
}

/**************************************************************
函数名： GrayTransTabReset

定义 ：  灰度变换界面重置

入口参数 ：	
	

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::GrayTransTabReset()
{
	
	// 关闭图像处理
	bIsUpdatedDisplay = false;
	
	// 关闭控件可用性
	EnableGrayTransCtrl(false);

	// 直方图图像重置
	m_GrayTransUI.pChartGray->removeAllSeries();

	// 设置为初始值
	m_GrayTransUI.pSpinBottom->setValue(0);
	m_GrayTransUI.pSpinTop->setValue(255);
	m_GrayTransUI.pSpinR->setValue(1);
	
	// 开启图像处理
	bIsUpdatedDisplay = true;

	return;
}

/**************************************************************
函数名： DisplaySwitch

定义 ：  显示模式转化

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::DisplaySwitch()
{
	// 参数声明
	int iRet;
	int iMdiHeight;
	int iMdiWidth;
	QString StrPath;
	//QWidget *pWidget_1;						// 探测器显示窗口1
	//QWidget *pWidget_2;						// 探测器显示窗口2
	//QWidget *pSubWidget;
	ImageWidget *pWidget_1;
	ImageWidget *pWidget_2;
	ImageWidget *pSubWidget;
	QLabel *pLabel_1;
	QLabel *pLabel_2;
	QString StrLabel_1;
	QString StrLabel_2;

	// 
	switch (m_DisplayMode)
	{
		// 探测器显示
		case DisplayMode_Detector:
		{
			// 删除之前的控件
			if (nullptr != pScreenMdiArea)
			{
				pScreenMdiArea->closeAllSubWindows();
				delete pScreenMdiArea;
				pScreenMdiArea = nullptr;
			}
			if (nullptr != pScreenSplitter)
			{
				pScreenSplitter->close();
				delete pScreenSplitter;
				pScreenSplitter = nullptr;
			}

			// 初始化Spliter
			pScreenSplitter = new QSplitter(Qt::Horizontal, ui.centralWidget);
			pScreenSplitter->setGeometry(0, 0,
				ui.centralWidget->width() - 30,
				ui.centralWidget->height());
			pScreenSplitter->setMouseTracking(true);
			
			// 载入探测器1图像
			if (false == IsDetInstanceValid(Detector_1))    // 探测器1没有图像实例
			{
				pWidget_1 = new ImageWidget(pScreenSplitter);
				pWidget_1->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");
				pWidget_1->setFixedSize(
					pScreenSplitter->width() / 2, 
					pScreenSplitter->height());
				StrLabel_1 = "Detector 1 Empty";
				pLabel_1 = new QLabel(pWidget_1);
				pLabel_1->setGeometry(0, 0, pWidget_1->width(), 20);
				pLabel_1->setText("Detector 1");
				pLabel_1->setAlignment(Qt::AlignCenter);
				pWidget_1->setWindowTitle(StrLabel_1);
				pScreenSplitter->addWidget(pWidget_1);
			}
			else                     // 探测器1有图像实例
			{
				// 初始化
				pImageDisplay.reset(new ImageDisplay()); 
				pWidget_1 = new ImageWidget();
				pLabel_1 = new QLabel(pWidget_1);
				
				// 窗口参数设置
				pWidget_1->setWindowTitle(PairDetIns_1.first);
				pWidget_1->setGeometry(0, 0, 
					pScreenSplitter->width() / 2, 
					pScreenSplitter->height());
				pLabel_1->setText(PairDetIns_1.first);
				pLabel_1->setGeometry(0, 0, pWidget_1->width(), 20);
				pLabel_1->setAlignment(Qt::AlignCenter);
				
				// 图像实例转化为Widget
				iRet = pImageDisplay.get()->InstanceToDisplay(PairDetIns_1.second, pWidget_1);
				if (SUCCESS != iRet) return;

				// 加载窗口至Splitter
				pScreenSplitter->addWidget(pWidget_1);
			}

			// 载入探测器2图像
			if (false == IsDetInstanceValid(Detector_2))    // 探测器2没有图像实例
			{
				pWidget_2 = new ImageWidget(pScreenSplitter);
				pWidget_2->setStyleSheet("background-color:white;border: 1px solid #C4C4C3;");
				pWidget_2->setFixedSize(
					pScreenSplitter->width() / 2,
					pScreenSplitter->height());
				StrLabel_2 = "Detector 2 Empty";
				pLabel_2 = new QLabel(pWidget_2);
				pLabel_2->setGeometry(0, 0, pWidget_2->width(), 20);
				pLabel_2->setText("Detector 2");
				pLabel_2->setAlignment(Qt::AlignCenter);
				pWidget_2->setWindowTitle(StrLabel_2);
				pScreenSplitter->addWidget(pWidget_2);
			}
			else                     // 探测器1有图像实例
			{
				// 初始化
				pImageDisplay.reset(new ImageDisplay());
				pWidget_2 = new ImageWidget();
				pLabel_2 = new QLabel(pWidget_2);

				// 窗口参数设置
				pWidget_2->setWindowTitle(PairDetIns_2.first);
				pWidget_2->setGeometry(0, 0,
					pScreenSplitter->width() / 2,
					pScreenSplitter->height());
				pLabel_2->setText(PairDetIns_2.first);
				pLabel_2->setGeometry(0, 0, pWidget_2->width(), 20);
				pLabel_2->setAlignment(Qt::AlignCenter);

				// 图像实例转化为Widget
				iRet = pImageDisplay.get()->InstanceToDisplay(PairDetIns_2.second, pWidget_2);
				if (SUCCESS != iRet) return;

				// 加载窗口至Splitter
				pScreenSplitter->addWidget(pWidget_2);
			}

			// 将1号探测器设置为选中
			SeletctDet(Detector_1);

			break;
		}

		// 列表显示
		case DisplayMode_ImageList:
		{
			// 删除之前的控件
			if (nullptr != pScreenMdiArea)
			{
				delete pScreenMdiArea;
				pScreenMdiArea = nullptr;
			}
			if (nullptr != pScreenSplitter)
			{
				pScreenSplitter->close();
				delete pScreenSplitter;
				pScreenSplitter = nullptr;
			}

			// 初始化
			pScreenMdiArea = new QMdiArea(ui.centralWidget);
			
			// 参数设置
			pScreenMdiArea->setGeometry(0, 0,
				ui.centralWidget->width(),
				ui.centralWidget->height());
			pScreenMdiArea->setViewMode(QMdiArea::TabbedView);   // 设置为Tab页面显示
			pScreenMdiArea->setTabsClosable(true);               // 设置为Tab页面显示 	
			pScreenMdiArea->setMouseTracking(true);
			iMdiHeight = pScreenMdiArea->height();
			iMdiWidth = pScreenMdiArea->width();
			
			// 显示
			pScreenMdiArea->show();

			// 遍历并显示所有图像
			for (int i = 0; i < MapImageIns.Size(); i++)
			{
				// 当前图像实例赋值
				PairCurrImageIns.first = MapImageIns.KeyAt(i);
				PairCurrImageIns.second = MapImageIns.ValueAt(i);
				
				// 初始化
				pSubWidget = new ImageWidget();
				pImageDisplay.reset(new ImageDisplay());

				// 参数设置
				pSubWidget->setGeometry(0, 0, iMdiWidth, iMdiHeight - 25);
				pSubWidget->setWindowTitle(PairCurrImageIns.first);
				
				// 图像实例转化为Widget
				iRet = pImageDisplay.get()->InstanceToDisplay(PairCurrImageIns.second, pSubWidget);
				if (SUCCESS != iRet) return;

				// 加载子窗口
				pScreenMdiArea->addSubWindow(pSubWidget);
				pSubWidget->show();
				pSubWidget->setWindowState(Qt::WindowMaximized);
				connect(pSubWidget, &ImageWidget::ImgWidgetClose, this, &ZCIPSMainFrame::DeleteInstance);
			}

			// 信号-槽函数
			connect(pScreenMdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), 
				this, SLOT(MdiAreaSwitch(QMdiSubWindow*)));

			break;
		}
		
		case DisplayMode_None:
		default: 
		{
			break;
		}
			
	}

	return;
}

/**************************************************************
函数名： UpdatePanel

定义 ：  更新Panel显示
		 该更新用于撤销与重做或其他来自于Panel外部的对Panel的更新需要

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::UpdatePanel()
{
	// 参数声明
	bool bRet;
	ProcessType m_PType;
	PanelTab m_PanelTab;
	ImageProcessTab m_ImageProcessTab;

	// 判断是否存在当前图像实例
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;

	// 判断是否存在当前图像处理
	bRet = PairCurrImageIns.second.get()->IsCurrProcessValid();

	// 若存在当前图像处理，则Panel应当跳转到对应的页面
	// 若不存在，则仅对当前显示的页面进行更新
	if (true == bRet)
	{
		// 获取处理类型
		m_PType = PairCurrImageIns.second.get()
			->pImageProcessIns.get()->PairCurrProcess.first;

		// 跳转页面选择
		switch (m_PType)
		{
			// 灰度变换
			case ProcessType_GrayTrans:
			{
				// 取消更新图像实例
				bIsUpdateImageIns = false;
				
				// 切换至图像处理Tab
				SwitchPanelTab(PanelTab_ImagePro);

				// 切换至灰度变换Tab
				SwitchImageProcessTab(ImageProcessTab_GrayTrans);
				
				// 开启更新图像实例
				bIsUpdateImageIns = true;

				break;
			}
			
			// 图像增强 
			case ProcessType_Enhancement:
			{
				// 取消更新图像实例
				bIsUpdateImageIns = false;
				
				// 切换至图像处理Tab
				SwitchPanelTab(PanelTab_ImagePro);

				// 切换至图像增强Tab
				SwitchImageProcessTab(ImageProcessTab_Enhancement);
				
				// 开启更新图像实例
				bIsUpdateImageIns = true;

				break;
			}
			
			case ProcessType_None:
			default:
			{
				return;
			}
		}
	}
	else
	{
		// 获取当前选择的 Panel Tab
		m_PanelTab = (PanelTab)ui.PanelTabWidget->currentIndex();

		// Panel Tab
		switch (m_PanelTab)
		{
			// 控制页面Tab
			case PanelTab_Control:
			{
				
				
				break;
			}
			
			// 图像处理Tab
			case PanelTab_ImagePro:
			{
				// 获取当前选择的 Image Process Tab
				m_ImageProcessTab = (ImageProcessTab)ui.ImageProcessTabWidget->currentIndex();
				
				// 当前选择的图像处理页面Tab
				switch (m_ImageProcessTab)
				{
					// 灰度变换
					case ImageProcessTab_GrayTrans:
					{
						// 灰度变换更新
						GrayTransTabUpdate();

						// 宏界面更新
						MarcoTabUpdate();
						
						break;
					}
					
					// 图像增强
					case ImageProcessTab_Enhancement:
					{
						// 宏界面更新
						MarcoTabUpdate();
						
						break;
					}
					
					default:
					{
						// 宏界面更新
						MarcoTabUpdate();
						
						break;
					}	
				}

				break;
			}
				
			default:
			{
				break;
			}
				
		}
	}
	
	return;
}

/**************************************************************
函数名： GetCurrImageInstance

定义 ：  获取当前显示的图像实例

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::GetCurrImageInstance()
{
	// 参数声明
	bool bRet;									// 函数返回值
	//QWidget* pSubWidget;						// 子窗口
	QWidget *pSubWidget;
	QSharedPointer<ImageInstance> pImageIns;    // 图像实例指针
	
	// 参数赋值
	pImageIns.clear();
	pSubWidget = nullptr;

	// 显示模式判断
	// 获取QWidget指针
	switch (m_DisplayMode)
	{
		// 探测器显示模式
		case DisplayMode_Detector:
		{
			switch (m_DetectorNo)
			{	
				// 探测器1号
				case Detector_1:
				{
					// 判断探测器实例是否存在
					// 不存在则退出
					bRet = IsDetInstanceValid(m_DetectorNo);
					if (false == bRet) return;

					// 当前图像实例赋值
					PairCurrImageIns.first = PairDetIns_1.first;
					PairCurrImageIns.second = PairDetIns_1.second;

					break;
				}

				// 探测器2号
				case Detector_2:
				{
					// 判断探测器实例是否存在
					// 不存在则退出
					bRet = IsDetInstanceValid(m_DetectorNo);
					if (false == bRet) return;

					// 当前图像实例赋值
					PairCurrImageIns.first = PairDetIns_2.first;
					PairCurrImageIns.second = PairDetIns_2.second;

					break;
				}
				default:
				{
					break;
				}	
			}
			
			break;
		}
		
		// 图像列表显示模式
		case DisplayMode_ImageList:
		{
			// 获取当前正在处理的图像指针
			// 若获取为空指针，则退出
			pSubWidget = pScreenMdiArea->activeSubWindow();
			if (nullptr == pSubWidget) return;
			
			// 
			pImageIns = MapImageIns.Find(pSubWidget->windowTitle());
			if (true == pImageIns.isNull())   // 未找到任何处理图像实例
			{
				PairCurrImageIns.first.clear();
				PairCurrImageIns.second.clear();
			}
			else							  // 当前处理实例赋值
			{
				PairCurrImageIns.first = pSubWidget->windowTitle();
				PairCurrImageIns.second = pImageIns;
			}

			break;
		}
		
		default:
		{
			break;
		}
		
	}

	return;
}

/**************************************************************
函数名： IsCurrImageInstanceValid

定义 ：  判断当前处理图像实例是否存在

入口参数 ：

函数返回值 ：
	true ：		存在
	false ：	不存在
****************************************************************/
bool ZCIPSMainFrame::IsCurrImageInstanceValid()
{
	if ((true == PairCurrImageIns.first.isEmpty())
		|| (true == PairCurrImageIns.second.isNull()))
	{
		return false;
	}
	else
	{
		return true;
	}
}

/**************************************************************
函数名： IsCurrProValChanged

定义 ：  当前处理是否存在参数改变

入口参数 ：

函数返回值 ：
	true ：		存在
	false ：	不存在
****************************************************************/
bool ZCIPSMainFrame::IsCurrProValChanged()
{
	// 参数声明
	bool bRet;
	ProcessType PType;
	
	// 类型获取
	PType = PairCurrImageIns.second.get()
		->pImageProcessIns.get()->PairCurrProcess.first;

	// 处理类型判断
	switch (PType)
	{
		// 图像变换
		case ProcessType_GrayTrans:
		{
			// 灰度变换指针赋值
			GrayTransProcess *pGTPro = 
				(GrayTransProcess *)PairCurrImageIns.second.get()
					->pImageProcessIns.get()->PairCurrProcess.second;
			
			// 判断当前处理是否存在有效的参数修改值
			bRet = pGTPro->bIsValChanged();
			if (false == bRet) return false;
			else return true;

			break;
		}
		
		// 图像增强
		case ProcessType_Enhancement: break;
			
		// 其他
		case ProcessType_None:
		default:
		{
			break;
		}
			
	}

	return true;
}

/**************************************************************
函数名： IsDetInstanceValid

定义 ：  判断探测器图像实例是否存在

入口参数 ：
	DetectorNo DetNo : 探测器编号

函数返回值 ：
	true ：		存在
	false ：	不存在
****************************************************************/
bool ZCIPSMainFrame::IsDetInstanceValid(DetectorNo DetNo)
{
	switch (DetNo)
	{
		// 1号探测器
		case Detector_1:
		{
			
			
			if ((true == PairDetIns_1.first.isEmpty())
				|| (true == PairDetIns_1.second.isNull()))
			{
				return false;
			}
			else
			{
				return true;
			}	
		}

		// 2号探测器
		case Detector_2:
		{
			if ((true == PairDetIns_2.first.isEmpty())
				|| (true == PairDetIns_2.second.isNull()))
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		
		default:
		{
			return false;
		}
	}
}

/**************************************************************
函数名： InitialImgDisplay

定义 ：  初始化图像显示

入口参数 ：

函数返回值 ：

****************************************************************/
void ZCIPSMainFrame::InitialImgDisplay()
{
	// 参数赋值
	pScreenSplitter = nullptr;
	pScreenMdiArea = nullptr;

	// 显示模式设置
	m_DisplayMode = DisplayMode_None;

	// 切换显示
	DisplaySwitch();
}
/***********************************************************************/
/*************************** 数据库处理 start ************************/
/********************************************************************
函数名：DataBaseUIInit

函数定义：数据库界面设计并初始化

入口参数：

函数返回值：
********************************************************************/
void ZCIPSMainFrame::DataBaseUIInit()
{
	//对tableview进行设置
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);//单选
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//以行为选择标准
	ui.tableView->horizontalHeader()->setHighlightSections(false);
	ui.tableView->horizontalHeader()->setBackgroundRole(QPalette::Background);
	ui.tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(255,255,255)};");
	ui.tableView->setEditTriggers(QAbstractItemView::SelectedClicked);
	//数据库连接
	DBProcess.reset(new DataBaseProcess);
	if (!DBProcess.get()->ConnectDB())
	{
		ui.DisplyDBinfolabel->setText("数据库连接失败！");
		return;
	}
	DBProcess.get()->sDBFilePath = "C:/Users/yyq/Desktop/tif";//需要修改的路径，此路径用于自己测试
	//DBProcess.get()->InsertDBdata();
	ui.DisplyDBinfolabel->setText("数据库连接成功！");
	connect(ui.DBcheckButton, SIGNAL(clicked()), this, SLOT(SearchDataBase()));
	connect(ui.UpdateDBButton, SIGNAL(clicked()), this, SLOT(UpdateDataBase()));
	connect(ui.SubmitButton, SIGNAL(clicked()), this, SLOT(SubmmitDataBase()));
	connect(ui.DeleteRecButton, SIGNAL(clicked()), this, SLOT(DeleteDataBaseEntry()));
}
/****************************************************************
函数名：UpdateDBdisplyInfo

函数定义：随着鼠标点击更新显示信息框

入口参数：

函数返回值：
********************************************************************/
void ZCIPSMainFrame::UpdateDBdisplyInfo(const QModelIndex index)
{
	QModelIndex index1 = ui.tableView->currentIndex();
	if (index1.isValid())
	{
		int rownum = ui.tableView->currentIndex().row();
		ui.DisplyDBinfolabel->setText("选中记录" + DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 0)).toString() + "!");
		ui.Filenamelabel->setText(DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 0)).toString());
		ui.Inspecdatelabel->setText(DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 1)).toString());
		ui.Inspecpersonlabel->setText(DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 2)).toString());
		ui.Spiecenamelabel->setText(DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 3)).toString());
		ui.Fileexistlabel->setText(DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 4)).toString());
	}
}
/***************************************************************
函数名：SearchDataBase

函数定义：根据检索条件查询数据库

入口参数：

函数返回值：
******************************************************************/
void ZCIPSMainFrame::SearchDataBase()
{
	DBProcess.get()->model->setTable("ImgTable");
	QString strSqlqurry = "";
	QString temppstr;
	if (ui.InspectcheckBox->isChecked())
	{
		temppstr = "检测日期 between '" + ui.dateEdit1->date().toString("yyyy-MM-dd") + "' and '" + ui.dateEdit2->date().toString("yyyy-MM-dd") + "'";
		strSqlqurry = strSqlqurry + temppstr;
	}
	if (ui.InspPersonEdit->text() != "")
	{
		if (strSqlqurry == "")
		{
			temppstr = " 检测人员 like '" + ui.InspPersonEdit->text() + "'";
		}
		else
		{
			temppstr = "and 检测人员 like '" + ui.InspPersonEdit->text() + "'";
		}
		strSqlqurry = strSqlqurry + temppstr;

	}
	if (ui.SpieceNameEdit->text() != "")
	{
		if (strSqlqurry == "")
		{
			temppstr = "工件名称 like '" + ui.SpieceNameEdit->text() + "'";
		}
		else
		{
			temppstr = "and 工件名称 like '" + ui.SpieceNameEdit->text() + "'";
		}
		strSqlqurry = strSqlqurry + temppstr;
	}
	DBProcess.get()->model->setFilter(strSqlqurry);
	DBProcess.get()->model->select();
	ui.DisplyDBinfolabel->setText("共找到" + QString::number(DBProcess.get()->model->rowCount(), 10) + "条符合条件的记录！");
	ui.tableView->setModel(DBProcess.get()->model);
	ui.tableView->resizeColumnsToContents();
	//tableview必须在setmodel以后才能进行信号槽连接
	connect(ui.tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(UpdateDBdisplyInfo(const QModelIndex &)));
	connect(ui.tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(OpenSelectedFile(const QModelIndex &)));
	//每次在重新检索数据库以后，将信息显示框置空
	ui.Filenamelabel->setText("");
	ui.Inspecdatelabel->setText("");
	ui.Inspecpersonlabel->setText("");
	ui.Spiecenamelabel->setText("");
	ui.Fileexistlabel->setText("");
}
/***********************************************************************
函数名：UpdateDataBase

函数定义：更新数据库内容

入口参数：

函数返回值：
************************************************************************/
void ZCIPSMainFrame::UpdateDataBase()
{
	ui.DisplyDBinfolabel->setText("数据库正在更新！");
	DBProcess.get()->UpdateDBdata();
	DBProcess.get()->model->setTable("ImgTable");
	DBProcess.get()->model->select();
	ui.tableView->setModel(DBProcess.get()->model);
	ui.tableView->resizeColumnsToContents();
	ui.DisplyDBinfolabel->setText("数据库更新完毕！");
}
/********************************************************************
函数名：DeleteDataBaseEntry()

函数定义：删除数据库中当前选中项

入口参数：

函数返回值：
***********************************************************************/
void ZCIPSMainFrame::DeleteDataBaseEntry()
{
	int currentrow = ui.tableView->currentIndex().row();
	DBProcess.get()->DeleteDBdata(currentrow);
	//删除记录后更新显示
	ui.DisplyDBinfolabel->setText("删除记录成功！");
	ui.Filenamelabel->setText("");
	ui.Inspecdatelabel->setText("");
	ui.Inspecpersonlabel->setText("");
	ui.Spiecenamelabel->setText("");
	ui.Fileexistlabel->setText("");
}
/**********************************************************************
函数名：SubmmitDataBase

函数定义：提交修改到数据库

入口参数：

函数返回值：
*************************************************************************/
void ZCIPSMainFrame::SubmmitDataBase()
{
	if (DBProcess.get()->SubmitDBdata())
	{
		ui.DisplyDBinfolabel->setText("提交修改到数据库成功！");
	}
	else
	{
		ui.DisplyDBinfolabel->setText("提交修改到数据库失败！");
	}
}
/***********************************************************************
函数名：OpenSelectedFile

函数定义：打开选中的数据库中的文件

入口参数：QModelIndex index  选中的记录在列表中的位置

函数返回值：
************************************************************************/
void ZCIPSMainFrame::OpenSelectedFile(const QModelIndex index)
{
	QModelIndex index1 = ui.tableView->currentIndex();
	if (index1.isValid())
	{
		int rownum = index1.row();
		QString StrFileName;						// 文件路径全名
		// 获取当前选中记录的文件路径全名
		StrFileName = DBProcess.get()->model->data(DBProcess.get()->model->index(rownum, 0)).toString();
		// 参数声明
		int iRet;									// 函数返回值
		int iHeight;
		int iWidth;
		QSharedPointer<ImageInstance> pImageIns;    // 图像实例
		/*
		if (pScreenSplitter != NULL)
		{
			pScreenSplitter->close();
			delete pScreenSplitter;
			pScreenSplitter = NULL;
		}
		if (pScreenMdiArea == NULL)
		{
			pScreenMdiArea = new QMdiArea;
			this->setCentralWidget(pScreenMdiArea);
			pScreenMdiArea->setGeometry(0, 0, MdiAreaWidth, MdiAreaHeight);
			pScreenMdiArea->setMouseTracking(true);
			pScreenMdiArea->setViewMode(QMdiArea::TabbedView);
			pScreenMdiArea->setTabsClosable(true);

		}*/
		// 图像实例重置
		pImageIns.reset(new ImageInstance);

		// 图像尺寸与控件尺寸适配
		//pImageIns.get()->ResizeWidget(ui.ImageMdiArea->width(), ui.ImageMdiArea->height());
		//int ww = pScreenMdiArea->width();
		//int hh = pScreenMdiArea->height();
		//pImageIns.get()->ResizeWidget(pScreenMdiArea->width(), pScreenMdiArea->height());
		// 图像实例加载

		iRet = pImageIns.get()->LoadFromFile(StrFileName);
		if (SUCCESS != iRet) return;

		// 加载入图像实例列表
		MapImageIns.Insert(StrFileName, pImageIns);

		// 切换显示模式
		m_DisplayMode = DisplayMode_ImageList;
		DisplaySwitch();

		// 获取当前的图像
		GetCurrImageInstance();




		// 更新区域掩码
		pAreaSelect.clear();

		// 更新Panel界面
		UpdatePanel();
		ui.DisplyDBinfolabel->setText("图像打开成功！");

	}
}
/********************************************** 数据库处理 end ***************************************************/

/****************************************** 图像标记 start *******************************/
/***************************************************************
函数名称：ItemToImageMark

函数定义：将图元转换为标记类

入口参数：QGraphicsItem*item    图元

函数返回值：MarksBase*    标记类
*****************************************************************/
MarksBase*ZCIPSMainFrame::ItemToImageMark(QGraphicsItem*item)
{
	MarksBase*pTempMark = new MarksBase;
	switch (item->type())
	{
	case QGraphicsRectItem::Type:
	{
		MarkRect*ptemrect = new MarkRect;
		ptemrect->type = MarkType_rect;
		ptemrect->tooltip = item->toolTip();
		ptemrect->rect = qgraphicsitem_cast<QGraphicsRectItem*>(item)->rect();
		ptemrect->pen = qgraphicsitem_cast<QGraphicsRectItem*>(item)->pen();
		pTempMark = (MarksBase*)ptemrect;
		pTempMark->tooltip = item->toolTip();
		pTempMark->type = MarkType_rect;
	}
	break;
	case QGraphicsEllipseItem::Type:
	{
		MarkEllipse*ptemellipse = new MarkEllipse;
		ptemellipse->type = MarkType_ellipse;
		ptemellipse->tooltip = item->toolTip();
		ptemellipse->rect = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)->rect();
		ptemellipse->pen = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)->pen();
		pTempMark = (MarksBase*)ptemellipse;
		pTempMark->tooltip = item->toolTip();
		pTempMark->type = MarkType_ellipse;
	}
	break;
	case QGraphicsPolygonItem::Type:
	{
		MarkPolygon*ptempolygon = new MarkPolygon;
		ptempolygon->type = MarkType_polygon;
		ptempolygon->tooltip = item->toolTip();
		ptempolygon->polygon = qgraphicsitem_cast<QGraphicsPolygonItem*>(item)->polygon();
		ptempolygon->pen = qgraphicsitem_cast<QGraphicsPolygonItem*>(item)->pen();
		pTempMark = (MarksBase*)ptempolygon;
		pTempMark->tooltip = item->toolTip();
		pTempMark->type = MarkType_polygon;
	}
	break;
	case QImageRuler::Type:
	{
		MarkRuler*ptemruler = new MarkRuler;
		ptemruler->type = MarkType_Ruler;
		ptemruler->tooltip = item->toolTip();
		ptemruler->StrartPoint = qgraphicsitem_cast<QImageRuler*>(item)->StartPoint;
		ptemruler->EndPoint = qgraphicsitem_cast<QImageRuler*>(item)->EndPoint;
		ptemruler->fHPixPropotion = qgraphicsitem_cast<QImageRuler*>(item)->fHPixProportion;
		ptemruler->fWPixPropotion = qgraphicsitem_cast<QImageRuler*>(item)->fVPixProportion;
		ptemruler->pen = qgraphicsitem_cast<QImageRuler*>(item)->pen();
		pTempMark = (MarksBase*)ptemruler;
		pTempMark->tooltip = item->toolTip();
		pTempMark->type = MarkType_Ruler;
	}
	break;
	default:
		break;
	}
	return pTempMark;
}
/**********************************************************************
函数名称：MarklistAdd

函数定义：将图元添加进标记列表

入口参数：QGraphicsItem*item     图元

函数返回值：
**********************************************************************/
void ZCIPSMainFrame::MarklistAdd(QGraphicsItem*item)
{
	MarksBase*pTempMark = ItemToImageMark(item);
	PairCurrImageIns.second->pImageMarks->MarkListAppend(pTempMark);
	PairCurrImageIns.second->mouseType = Mouse_Default;
}
/**********************************************************************
函数名称：MarklistEdit

函数定义：编辑标记列表中的元素

入口参数：QGraphicsItem*item     图元

函数返回值：
**********************************************************************/
void ZCIPSMainFrame::MarklistEdit(QGraphicsItem*item)
{
	MarksBase*pTempMark = ItemToImageMark(item);
	PairCurrImageIns.second->pImageMarks->MarkListEdit(pTempMark);
}
/*********************************************************************
函数名称：GetActWinandCurrentIns

函数定义：获得当前active窗口和当前实例

入口参数：

函数返回值：
***********************************************************************/
void ZCIPSMainFrame::GetActWinandCurrentIns()
{
	QMdiSubWindow* pSubWidget;
	ImageScene *pImageScene;
	QGraphicsView *pImageView;
	pSubWidget = pScreenMdiArea->activeSubWindow();
	if (pSubWidget == NULL)return;
	pImageView = pSubWidget->findChild<QGraphicsView *>(QStringLiteral("ImageView"));
	if (pImageView != nullptr)
	{
		pImageScene = (ImageScene*)pImageView->scene();
		pImageScene->mouseType = PairCurrImageIns.second->mouseType;
		if (pImageScene->mouseType == Mouse_Default)
		{
			for (int i = 0; i < pImageScene->selectedItems().count(); i++)
			{
				pImageScene->selectedItems().at(i)->setSelected(false);
			}
		}
		connect(pImageScene, &ImageScene::AddNewItem, this, &ZCIPSMainFrame::MarklistAdd);
		connect(pImageScene, &ImageScene::EditItems, this, &ZCIPSMainFrame::MarklistEdit);
	}
}
/********************************************************************
函数名称：MarkChangeToRect

函数定义：将鼠标做的标记类型转换为矩形

入口参数：

函数返回值：
*******************************************************************/
void ZCIPSMainFrame::MarkChangeToRect()
{
	// 判断是否存在当前图像实例
	bool bRet;
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;
	PairCurrImageIns.second->mouseType = Mouse_Rect;
	GetActWinandCurrentIns();
}
/*******************************************************************
函数名称：MarkCHangeToEllipse

函数定义：将当前鼠标标记类型转换为椭圆

入口参数：

函数返回值：
*******************************************************************/
void ZCIPSMainFrame::MarkCHangeToEllipse()
{
	// 判断是否存在当前图像实例
	bool bRet;
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;
	PairCurrImageIns.second->mouseType = Mouse_Ellipse;
	GetActWinandCurrentIns();
}
/******************************************************************
函数名称：MarkChangeToPolygon

函数定义：将当前鼠标标记类型转换为多边形

入口参数：

函数返回值：
*****************************************************************/
void ZCIPSMainFrame::MarkChangeToPolygon()
{
	// 判断是否存在当前图像实例
	bool bRet;
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;
	PairCurrImageIns.second->mouseType = Mouse_Polygon;
	GetActWinandCurrentIns();
}
/*********************************************************************
函数名称：MarkCancelMouseSelect

函数定义：鼠标标记类型转换为无

入口参数：

函数返回值：
***********************************************************************/
void ZCIPSMainFrame::MarkCancelMouseSelect()
{
	// 判断是否存在当前图像实例
	bool bRet;
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;
	PairCurrImageIns.second->mouseType = Mouse_Default;
	GetActWinandCurrentIns();
}
void ZCIPSMainFrame::closeEvent(QCloseEvent * event)
{
	ctScanHandle->ctScanWidgetClosed();
}
/*********************************************************************
函数名称：MarkChangeToRuler

函数定义：将鼠标标记类型转换为长度测量

入口参数：

函数返回值：
*******************************************************************/
void ZCIPSMainFrame::MarkChangeToRuler()
{
	// 判断是否存在当前图像实例
	bool bRet;
	bRet = IsCurrImageInstanceValid();
	if (true != bRet) return;
	PairCurrImageIns.second->mouseType = Mouse_Ruler;
	GetActWinandCurrentIns();
}
/****************************************** 图像标记 end ************************************/