#pragma once

//#include <QStyle>
//#include <QSlider>
//#include <QStylePainter>
//#include <QStyleOptionSlider>
//#include <QAbstractSlider>

/******************************** 枚举类型 ***************************************/
// 滑块
enum SpanHandle
{
	NoHandle,
	LowerHandle,
	MidHandle,
	UpperHandle
};

enum CtrlHandle
{
	LowCtrl = 0,
	MidCtrl,
	UpperCtrl
};

class TripleSlider : public QSlider
{
	Q_OBJECT

// 参数
private:
	SpanHandle m_SpanHandle;
	int iHandleVal[3];	// 0 -- slider下限（实际值）
			            // 1 -- slider中值（实际值）
						// 2 -- slider上限（实际值）
	int iOffset;		// slider中滑条至slider控件边缘的偏移值（像素）
	int iPosition;		
	SpanHandle LastHandlePressed;
	SpanHandle MainHandleControl;
	QStyle::SubControl CtrlPressed[3];   // 0 -- Lower下限滑块
	                                     // 1 -- Mid中限滑块
										 // 2 -- Mid上限滑块

// 响应与重载
public:

	// 初始化
	TripleSlider
	(
		Qt::Orientation orientation,
		QWidget* parent = nullptr
	);

	// 鼠标点击事件重载
	virtual void mousePressEvent(QMouseEvent* event);

	// 鼠标移动事件重载
	virtual void mouseMoveEvent(QMouseEvent* event);

	// 鼠标释放事件重载
	virtual void mouseReleaseEvent(QMouseEvent* event);

	// 重绘事件重载
	virtual void paintEvent(QPaintEvent* event);

// 信号
signals:
	// 下限值改变
	void LowerValueChanged();

	// 中限值改变
	void MidValueChanged();

	// 上限值改变
	void UpperValueChanged();

// 函数
public:
	// 绘制滑块
	void DrawHandle
	(
		QStylePainter* painter, 
		SpanHandle m_InHandle
	) const;

	// 初始化Slider Style
	void InitStyleOption
	(
		QStyleOptionSlider* pSliderOpt, 
		SpanHandle m_InHandle = UpperHandle
	) const;

	// 滑块点击处理事件
	void HandleMousePress
	(
		QPoint& PointPress,
		QStyle::SubControl& m_SubControl,
		int iValue,
		SpanHandle m_InHandle
	);

	// 
	int Pick(const QPoint& m_Point) const;

	// 像素值转换到实际值
	int PixelPosToRangeValue(int iPos) const;
	
	// 设置下限
	void SetLowerVal
	(
		int iVal,
		bool bIsUpdate
	);

	// 设置中限
	void SetMidVal
	(
		int iVal,
		bool bIsUpdate
	);

	// 设置上限
	void SetUpperVal
	(
		int iVal,
		bool bIsUpdate
	);

	// 获取下限
	int iGetLowerVal();

	// 获取中限
	int iGetMidVal();

	// 获取上限
	int iGetUpperVal();

};