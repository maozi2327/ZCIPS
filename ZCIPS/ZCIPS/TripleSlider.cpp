#include <QMouseEvent>
#include "TripleSlider.h"
#include "ERRInfo.h"


/**************************************************************
函数名： TripleSlider

定义 ：  构造函数

入口参数 ：

函数返回值 ：

****************************************************************/
TripleSlider::TripleSlider(Qt::Orientation orientation, QWidget* parent)
	:QSlider(orientation, parent),
	iPosition(0),
	iOffset(0),
	LastHandlePressed(NoHandle),
	MainHandleControl(LowerHandle)
{
	// 参数初始化
	this->setMinimum(0);
	this->setMaximum(255);
	this->setSingleStep(1);

	// 滑块初始化
	iHandleVal[LowCtrl] = 0;
	iHandleVal[MidCtrl] = 128;
	iHandleVal[UpperCtrl] = 255;
	CtrlPressed[LowCtrl] = QStyle::SC_None;
	CtrlPressed[MidCtrl] = QStyle::SC_None;
	CtrlPressed[UpperCtrl] = QStyle::SC_None;
}

/**************************************************************
函数名： mousePressEvent

定义 ：  鼠标点击响应

入口参数 ：
	QMouseEvent* event ：

函数返回值 ：

****************************************************************/
void TripleSlider::mousePressEvent(QMouseEvent* event)
{
	// 参数声明
	QSlider* pSliderPt;
	SpanHandle SpanHandleLoop;

	// 指针赋值
	pSliderPt = (QSlider*)this;

	// 入参判断
	if ((pSliderPt->minimum() == pSliderPt->maximum())
		|| (event->buttons() ^ event->button()))
	{
		event->ignore();
		return;
	}

	// 鼠标点击处理
	
	for (int i = 0; i < 3; i++)
	{
		// 获取当前处理的滑块
		switch (i)
		{
			case 0: {SpanHandleLoop = LowerHandle; break; }
			case 1: {SpanHandleLoop = MidHandle; break; }
			case 2:
			default: {SpanHandleLoop = UpperHandle; break; }
		}

		// 滑块点击处理
		HandleMousePress(event->pos(), CtrlPressed[i], iHandleVal[i], SpanHandleLoop);
		if (QStyle::SC_SliderHandle == CtrlPressed[i]) break;
	}
	
	// 
	event->accept();
}

/**************************************************************
函数名： mouseMoveEvent

定义 ：  鼠标移动事件重载

入口参数 ：
	QMouseEvent* event ：

函数返回值 ：

****************************************************************/
void TripleSlider::mouseMoveEvent(QMouseEvent* event)
{
	// 参数声明
	int iSize;
	int iNewPosition;
	QRect Rect;
	QStyleOptionSlider SliderStyleOpt;

	// 初始化 Slider Style Option
	initStyleOption(&SliderStyleOpt);

	// 未点击任何
	if ((QStyle::SC_SliderHandle != CtrlPressed[LowCtrl])
		&& (QStyle::SC_SliderHandle != CtrlPressed[MidCtrl])
		&& (QStyle::SC_SliderHandle != CtrlPressed[UpperCtrl]))
	{
		event->ignore();
		return;
	}

	// 获取拖动时鼠标与滚动条之间的最大距离
	iSize = this->style()->pixelMetric(
		QStyle::PM_MaximumDragDistance,
		&SliderStyleOpt, this);

	// 像素位置->实际值
	iNewPosition = PixelPosToRangeValue(Pick(event->pos()) - iOffset);
	if (iSize >= 0)
	{
		Rect = this->rect().adjusted(-iSize, -iSize, iSize, iSize);
		if (!Rect.contains(event->pos()))
		{
			iNewPosition = iPosition;
		}
	}

	// 滑块运行处理
	if (QStyle::SC_SliderHandle == CtrlPressed[LowCtrl])  // 下限运动处理
	{
		if (iNewPosition > (iHandleVal[UpperCtrl] - 10))
		{
			CtrlPressed[LowCtrl] = QStyle::SC_None;

			// 发出信号
			emit LowerValueChanged();
		}
		else if (iNewPosition < this->minimum())
		{
			CtrlPressed[LowCtrl] = QStyle::SC_None;

			// 发出信号
			emit LowerValueChanged();
		}
		else
		{
			SetLowerVal(iNewPosition, true);
		}
	}
	else if (QStyle::SC_SliderHandle == CtrlPressed[UpperCtrl])	// 上限运动处理
	{
		if (iNewPosition < iHandleVal[LowCtrl] + 10)
		{
			CtrlPressed[UpperCtrl] = QStyle::SC_None;

			// 发出信号
			emit UpperValueChanged();
		}
		else if (iNewPosition > this->maximum())
		{
			CtrlPressed[UpperCtrl] = QStyle::SC_None;

			// 发出信号
			emit UpperValueChanged();
		}
		else
		{
			SetUpperVal(iNewPosition, true);
		}
	}
	else if (QStyle::SC_SliderHandle == CtrlPressed[MidCtrl])	// 中限运动处理
	{
		if (iNewPosition < iHandleVal[LowCtrl] + 1)
		{
			CtrlPressed[MidCtrl] = QStyle::SC_None;

			// 发出信号
			emit MidValueChanged();
		}
		else if (iNewPosition > iHandleVal[UpperCtrl] - 1)
		{
			CtrlPressed[MidCtrl] = QStyle::SC_None;

			// 发出信号
			emit MidValueChanged();
		}
		else
		{
			SetMidVal(iNewPosition, true);
		}
	}

	event->accept();
}

/**************************************************************
函数名： mouseReleaseEvent

定义 ：  鼠标按键释放事件重载

入口参数 ：
	QMouseEvent* event ：

函数返回值 ：

****************************************************************/
void TripleSlider::mouseReleaseEvent(QMouseEvent* event)
{
	// 鼠标释放事件
	QSlider::mouseReleaseEvent(event);

	// 发出信号
	if (QStyle::SC_SliderHandle == CtrlPressed[LowCtrl])
	{
		emit LowerValueChanged();
	}
	else if (QStyle::SC_SliderHandle == CtrlPressed[MidCtrl])
	{
		emit MidValueChanged();
	}
	else if (QStyle::SC_SliderHandle == CtrlPressed[UpperCtrl])
	{
		emit UpperValueChanged();
	}
	
	// 按钮状态设置为未点击状态
	CtrlPressed[LowCtrl] = QStyle::SC_None;
	CtrlPressed[MidCtrl] = QStyle::SC_None;
	CtrlPressed[UpperCtrl] = QStyle::SC_None;
	
	// 更新显示
	this->update();
}

/**************************************************************
函数名： paintEvent

定义 ：  绘制事件重载

入口参数 ：
	QPaintEvent* event ： 绘制事件

函数返回值 ：
	
****************************************************************/
void TripleSlider::paintEvent(QPaintEvent* event)
{
	// 参数声明
	QStylePainter Painter(this);

	// 空置指针
	Q_UNUSED(event);

	// 滑块绘制
	DrawHandle(&Painter, UpperHandle);
	DrawHandle(&Painter, MidHandle);
	DrawHandle(&Painter, LowerHandle);

	return;
}

















/**************************************************************
函数名： DrawHandle

定义 ：  绘制单个滑块

入口参数 ：
	QStylePainter* pPainter ：
	SpanHandle m_InHandle ：

函数返回值 ：

****************************************************************/
void TripleSlider::DrawHandle
(
	QStylePainter* pPainter,
	SpanHandle m_InHandle
)const
{
	// 参数声明
	QStyleOptionSlider SliderOpt;
	QStyle::SubControl CtrlDrawPressed;

	// Slider Style 设置
	InitStyleOption(&SliderOpt, m_InHandle);
	SliderOpt.subControls = QStyle::SC_SliderHandle;

	// 待绘制滑块判断
	switch (m_InHandle)
	{
		case LowerHandle:
		{
			CtrlDrawPressed = CtrlPressed[LowCtrl];
			break;
		}
		case MidHandle:
		{
			CtrlDrawPressed = CtrlPressed[MidCtrl];
			break;
		}
		case NoHandle:
		case UpperHandle:
		default:
		{
			CtrlDrawPressed = CtrlPressed[UpperCtrl];
			break;
		}
			
	}

	// 滑块绘制
	if (QStyle::SC_SliderHandle == CtrlDrawPressed)
	{
		SliderOpt.activeSubControls = CtrlDrawPressed;
		SliderOpt.state |= QStyle::State_Sunken;
	}
	pPainter->drawComplexControl(QStyle::CC_Slider, SliderOpt);
}

/**************************************************************
函数名： InitStyleOption

定义 ：  初始化Slider Style

入口参数 ：
	StyleOptionSlider* option ：
	SpanHandle m_InHandle ：

函数返回值 ：

****************************************************************/
void TripleSlider::InitStyleOption
(
	QStyleOptionSlider* pSliderOpt,
	SpanHandle m_InHandle
)const
{
	// 参数声明
	QSlider* pSliderPt;     // QSlider的指针
	
	// 参数检查
	if (!pSliderOpt) return;

	// 指针赋值
	pSliderPt = (QSlider*)this;

	// Slider Style赋值
	pSliderOpt->initFrom(pSliderPt);
	pSliderOpt->subControls = QStyle::SC_None;
	pSliderOpt->activeSubControls = QStyle::SC_None;
	pSliderOpt->orientation = pSliderPt->orientation();
	pSliderOpt->maximum = pSliderPt->maximum();
	pSliderOpt->minimum = pSliderPt->minimum();
	pSliderOpt->tickPosition = pSliderPt->tickPosition();
	pSliderOpt->tickInterval = pSliderPt->tickInterval();
	pSliderOpt->upsideDown = (pSliderPt->orientation() == Qt::Horizontal) ?
		(pSliderPt->invertedAppearance() != (pSliderOpt->direction == Qt::RightToLeft))
		: (!pSliderPt->invertedAppearance());
	pSliderOpt->direction = Qt::LeftToRight;
	pSliderOpt->singleStep = pSliderPt->singleStep();
	pSliderOpt->pageStep = pSliderPt->pageStep();
	if (Qt::Horizontal == pSliderPt->orientation())
		pSliderOpt->state |= QStyle::State_Horizontal;

	// 位置与实际值赋值
	switch (m_InHandle)
	{
		case LowerHandle:
		{
			pSliderOpt->sliderPosition = iHandleVal[LowCtrl];
			pSliderOpt->sliderValue = iHandleVal[LowCtrl];
	
			break;
		}
		
		case MidHandle:
		{
			pSliderOpt->sliderPosition = iHandleVal[MidCtrl];
			pSliderOpt->sliderValue = iHandleVal[MidCtrl];

			break;
		}
		case NoHandle:
		case UpperHandle:
		default:
		{
			pSliderOpt->sliderPosition = iHandleVal[UpperCtrl];
			pSliderOpt->sliderValue = iHandleVal[UpperCtrl];

			break;
		}	
	}
}

/**************************************************************
函数名： HandleMousePress

定义 ：  滑块点击处理事件

入口参数 ：
	QPoint& PointPress ：
	QStyle::SubControl& m_SubControl ： 
	int iValue ：
	SpanHandle m_InHandle ：

函数返回值 ：

****************************************************************/
void TripleSlider::HandleMousePress
(
	QPoint& PointPress,
	QStyle::SubControl& m_SubControl,
	int iValue,
	SpanHandle m_InHandle
)
{
	// 参数声明
	QStyleOptionSlider SliderOpt;
	QStyle::SubControl CtrlOldControl;
	QRect m_Rect;
	QSlider* pSliderPt;					// QSlider的指针

	// Slider Style 设置
	InitStyleOption(&SliderOpt, m_InHandle);
	SliderOpt.subControls = QStyle::SC_SliderHandle;

	// 指针赋值
	pSliderPt = (QSlider*)this;

	// 判断是否点击到了确定的滑块上面
	CtrlOldControl = m_SubControl;
	m_SubControl = pSliderPt->style()->hitTestComplexControl(QStyle::CC_Slider, &SliderOpt, PointPress, pSliderPt);
	m_Rect = pSliderPt->style()->subControlRect(QStyle::CC_Slider, &SliderOpt, QStyle::SC_SliderHandle, pSliderPt);
	if (QStyle::SC_SliderHandle == m_SubControl)
	{
		iPosition = iValue;
		pSliderPt->setSliderDown(true);
	}
	else
	{
		m_SubControl = CtrlOldControl;
	}

	// 更新特殊区域的滑块
	if (m_SubControl != CtrlOldControl)
		pSliderPt->update(m_Rect);
}

/**************************************************************
函数名： Pick

定义 ：  

入口参数 ：
	const QPoint& m_Point ：

函数返回值 ：

****************************************************************/
int TripleSlider::Pick(const QPoint& m_Point) const
{
	return ((this->orientation() == Qt::Horizontal) ? m_Point.x() : m_Point.y());
}

/**************************************************************
函数名： PixelPosToRangeValue

定义 ：  像素位置转换到实际数据

入口参数 ：
	int iPos ： 像素位置

函数返回值 ：
	int iVal :  实际数据
****************************************************************/
int TripleSlider::PixelPosToRangeValue(int iPos) const
{
	// 参数声明
	int iSliderMin;
	int iSliderMax;
	int iSliderLength;
	QStyleOptionSlider SliderStyleOpt;
	QRect RectGroove;
	QRect RectHandle;
	QSlider* pSlider;

	// Slider Style Option初始化
	initStyleOption(&SliderStyleOpt);

	// 参数赋值
	iSliderMin = 0;
	iSliderMax = 0;
	iSliderLength = 0;

	// 指针赋值
	pSlider = (QSlider*)this;

	// 方位设置
	RectGroove = pSlider->style()->subControlRect(QStyle::CC_Slider, 
		&SliderStyleOpt, 
		QStyle::SC_SliderGroove, 
		pSlider);
	RectHandle = pSlider->style()->subControlRect(QStyle::CC_Slider, 
		&SliderStyleOpt, 
		QStyle::SC_SliderHandle, 
		pSlider);
	if (Qt::Horizontal == pSlider->orientation())
	{
		iSliderLength = RectHandle.width();
		iSliderMin = RectGroove.x();
		iSliderMax = RectGroove.right() - iSliderLength + 1;
	}
	else
	{
		iSliderLength = RectHandle.height();
		iSliderMin = RectGroove.y();
		iSliderMax = RectGroove.bottom() - iSliderLength + 1;
	}

	// 
	return QStyle::sliderValueFromPosition(pSlider->minimum(), 
		pSlider->maximum(), iPos - iSliderMin,
		iSliderMax - iSliderMin, SliderStyleOpt.upsideDown);
}

/**************************************************************
函数名： SetLowerVal

定义 ：  设置下限

入口参数 ：
	int iVal ：			下限设置值
	bool bIsUpdate ：   是否进行图像更新

函数返回值 ：

****************************************************************/
void TripleSlider::SetLowerVal
(
	int iVal, 
	bool bIsUpdate
)
{
	// 参数声明
	int iUpdateMid;
	float fRatio;

	// 入参检查
	if ((iVal < this->minimum()) || (iVal > iHandleVal[UpperCtrl] - 10)) return;

	// 中限比例计算
	iUpdateMid = iHandleVal[MidCtrl];
	fRatio = (float)(iUpdateMid - iHandleVal[LowCtrl]) 
		/ (float)(iHandleVal[UpperCtrl] - iHandleVal[LowCtrl]);
	
	// 下限设置
	iHandleVal[LowCtrl] = iVal;

	// 中限调整
	// 保持中限比例不变
	iUpdateMid = (int)round((float)iHandleVal[LowCtrl]
		+ fRatio * (float)(iHandleVal[UpperCtrl] - iHandleVal[LowCtrl]));
	SetMidVal(iUpdateMid, false);

	// 控件重绘
	if (true == bIsUpdate) update();
}

/**************************************************************
函数名： SetMidVal

定义 ：  设置中限

入口参数 ：
	int iVal ：			中限设置值
	bool bIsUpdate ：   是否进行图像更新

函数返回值 ：

****************************************************************/
void TripleSlider::SetMidVal
(
	int iVal,
	bool bIsUpdate
)
{
	// 入参检查
	if ((iVal <= iHandleVal[LowCtrl]) || (iVal >= iHandleVal[UpperCtrl])) return;

	// 中限设置
	iHandleVal[MidCtrl] = iVal;

	// 控件重绘
	if (true == bIsUpdate) update();
}

/**************************************************************
函数名： SetUpperVal

定义 ：  设置上限

入口参数 ：
	int iVal ：			上限设置值
	bool bIsUpdate ：   是否进行图像更新

函数返回值 ：

****************************************************************/
void TripleSlider::SetUpperVal
(
	int iVal,
	bool bIsUpdate
)
{
	// 参数声明
	int iUpdateMid;
	float fRatio;

	// 入参检查
	if ((iVal > this->maximum()) || (iVal < iHandleVal[LowCtrl] + 10)) return;

	// 中限比例计算
	iUpdateMid = iHandleVal[MidCtrl];
	fRatio = (float)(iUpdateMid - iHandleVal[LowCtrl]) 
		/ (float)(iHandleVal[UpperCtrl] - iHandleVal[LowCtrl]);

	// 上限设置
	iHandleVal[UpperCtrl] = iVal;

	// 中限调整
	// 保持中限比例不变
	iUpdateMid = (int)round((float)iHandleVal[LowCtrl] 
		+ fRatio * (float)(iHandleVal[UpperCtrl] - iHandleVal[LowCtrl]));
	SetMidVal(iUpdateMid, false);

	// 控件重绘
	if (true == bIsUpdate) update();
}

/**************************************************************
函数名： iGetLowerVal

定义 ：  获取下限

入口参数 ：

函数返回值 ：

****************************************************************/
int TripleSlider::iGetLowerVal()
{
	return iHandleVal[0];
}

/**************************************************************
函数名： iGetMidVal

定义 ：  获取中限

入口参数 ：

函数返回值 ：

****************************************************************/
int TripleSlider::iGetMidVal()
{
	return iHandleVal[1];
}

/**************************************************************
函数名： iGetMidVal

定义 ：  获取上限

入口参数 ：

函数返回值 ：

****************************************************************/
int TripleSlider::iGetUpperVal()
{
	return iHandleVal[2];
}
