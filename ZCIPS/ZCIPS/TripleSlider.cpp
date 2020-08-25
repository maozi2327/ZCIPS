
#include <QMouseEvent>
#include "TripleSlider.h"
#include "ERRInfo.h"


/**************************************************************
�������� TripleSlider

���� ��  ���캯��

��ڲ��� ��

��������ֵ ��

****************************************************************/
TripleSlider::TripleSlider(Qt::Orientation orientation, QWidget* parent)
	:QSlider(orientation, parent),
	iPosition(0),
	iOffset(0),
	LastHandlePressed(NoHandle),
	MainHandleControl(LowerHandle)
{
	// ������ʼ��
	this->setMinimum(0);
	this->setMaximum(255);
	this->setSingleStep(1);

	// �����ʼ��
	iHandleVal[LowCtrl] = 0;
	iHandleVal[MidCtrl] = 128;
	iHandleVal[UpperCtrl] = 255;
	CtrlPressed[LowCtrl] = QStyle::SC_None;
	CtrlPressed[MidCtrl] = QStyle::SC_None;
	CtrlPressed[UpperCtrl] = QStyle::SC_None;
}

/**************************************************************
�������� mousePressEvent

���� ��  �������Ӧ

��ڲ��� ��
	QMouseEvent* event ��

��������ֵ ��

****************************************************************/
void TripleSlider::mousePressEvent(QMouseEvent* event)
{
	// ��������
	QSlider* pSliderPt;
	SpanHandle SpanHandleLoop;

	// ָ�븳ֵ
	pSliderPt = (QSlider*)this;

	// ����ж�
	if ((pSliderPt->minimum() == pSliderPt->maximum())
		|| (event->buttons() ^ event->button()))
	{
		event->ignore();
		return;
	}

	// ���������
	
	for (int i = 0; i < 3; i++)
	{
		// ��ȡ��ǰ����Ļ���
		switch (i)
		{
			case 0: {SpanHandleLoop = LowerHandle; break; }
			case 1: {SpanHandleLoop = MidHandle; break; }
			case 2:
			default: {SpanHandleLoop = UpperHandle; break; }
		}

		// ����������
		HandleMousePress(event->pos(), CtrlPressed[i], iHandleVal[i], SpanHandleLoop);
		if (QStyle::SC_SliderHandle == CtrlPressed[i]) break;
	}
	
	// 
	event->accept();
}

/**************************************************************
�������� mouseMoveEvent

���� ��  ����ƶ��¼�����

��ڲ��� ��
	QMouseEvent* event ��

��������ֵ ��

****************************************************************/
void TripleSlider::mouseMoveEvent(QMouseEvent* event)
{
	// ��������
	int iSize;
	int iNewPosition;
	QRect Rect;
	QStyleOptionSlider SliderStyleOpt;

	// ��ʼ�� Slider Style Option
	initStyleOption(&SliderStyleOpt);

	// δ����κ�
	if ((QStyle::SC_SliderHandle != CtrlPressed[LowCtrl])
		&& (QStyle::SC_SliderHandle != CtrlPressed[MidCtrl])
		&& (QStyle::SC_SliderHandle != CtrlPressed[UpperCtrl]))
	{
		event->ignore();
		return;
	}

	// ��ȡ�϶�ʱ����������֮���������
	iSize = this->style()->pixelMetric(
		QStyle::PM_MaximumDragDistance,
		&SliderStyleOpt, this);

	// ����λ��->ʵ��ֵ
	iNewPosition = PixelPosToRangeValue(Pick(event->pos()) - iOffset);
	if (iSize >= 0)
	{
		Rect = this->rect().adjusted(-iSize, -iSize, iSize, iSize);
		if (!Rect.contains(event->pos()))
		{
			iNewPosition = iPosition;
		}
	}

	// �������д���
	if (QStyle::SC_SliderHandle == CtrlPressed[LowCtrl])  // �����˶�����
	{
		if (iNewPosition > (iHandleVal[UpperCtrl] - 10))
		{
			CtrlPressed[LowCtrl] = QStyle::SC_None;

			// �����ź�
			emit LowerValueChanged();
		}
		else if (iNewPosition < this->minimum())
		{
			CtrlPressed[LowCtrl] = QStyle::SC_None;

			// �����ź�
			emit LowerValueChanged();
		}
		else
		{
			SetLowerVal(iNewPosition, true);
		}
	}
	else if (QStyle::SC_SliderHandle == CtrlPressed[UpperCtrl])	// �����˶�����
	{
		if (iNewPosition < iHandleVal[LowCtrl] + 10)
		{
			CtrlPressed[UpperCtrl] = QStyle::SC_None;

			// �����ź�
			emit UpperValueChanged();
		}
		else if (iNewPosition > this->maximum())
		{
			CtrlPressed[UpperCtrl] = QStyle::SC_None;

			// �����ź�
			emit UpperValueChanged();
		}
		else
		{
			SetUpperVal(iNewPosition, true);
		}
	}
	else if (QStyle::SC_SliderHandle == CtrlPressed[MidCtrl])	// �����˶�����
	{
		if (iNewPosition < iHandleVal[LowCtrl] + 1)
		{
			CtrlPressed[MidCtrl] = QStyle::SC_None;

			// �����ź�
			emit MidValueChanged();
		}
		else if (iNewPosition > iHandleVal[UpperCtrl] - 1)
		{
			CtrlPressed[MidCtrl] = QStyle::SC_None;

			// �����ź�
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
�������� mouseReleaseEvent

���� ��  ��갴���ͷ��¼�����

��ڲ��� ��
	QMouseEvent* event ��

��������ֵ ��

****************************************************************/
void TripleSlider::mouseReleaseEvent(QMouseEvent* event)
{
	// ����ͷ��¼�
	QSlider::mouseReleaseEvent(event);

	// �����ź�
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
	
	// ��ť״̬����Ϊδ���״̬
	CtrlPressed[LowCtrl] = QStyle::SC_None;
	CtrlPressed[MidCtrl] = QStyle::SC_None;
	CtrlPressed[UpperCtrl] = QStyle::SC_None;
	
	// ������ʾ
	this->update();
}

/**************************************************************
�������� paintEvent

���� ��  �����¼�����

��ڲ��� ��
	QPaintEvent* event �� �����¼�

��������ֵ ��
	
****************************************************************/
void TripleSlider::paintEvent(QPaintEvent* event)
{
	// ��������
	QStylePainter Painter(this);

	// ����ָ��
	Q_UNUSED(event);

	// �������
	DrawHandle(&Painter, UpperHandle);
	DrawHandle(&Painter, MidHandle);
	DrawHandle(&Painter, LowerHandle);

	return;
}

















/**************************************************************
�������� DrawHandle

���� ��  ���Ƶ�������

��ڲ��� ��
	QStylePainter* pPainter ��
	SpanHandle m_InHandle ��

��������ֵ ��

****************************************************************/
void TripleSlider::DrawHandle
(
	QStylePainter* pPainter,
	SpanHandle m_InHandle
)const
{
	// ��������
	QStyleOptionSlider SliderOpt;
	QStyle::SubControl CtrlDrawPressed;

	// Slider Style ����
	InitStyleOption(&SliderOpt, m_InHandle);
	SliderOpt.subControls = QStyle::SC_SliderHandle;

	// �����ƻ����ж�
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

	// �������
	if (QStyle::SC_SliderHandle == CtrlDrawPressed)
	{
		SliderOpt.activeSubControls = CtrlDrawPressed;
		SliderOpt.state |= QStyle::State_Sunken;
	}
	pPainter->drawComplexControl(QStyle::CC_Slider, SliderOpt);
}

/**************************************************************
�������� InitStyleOption

���� ��  ��ʼ��Slider Style

��ڲ��� ��
	StyleOptionSlider* option ��
	SpanHandle m_InHandle ��

��������ֵ ��

****************************************************************/
void TripleSlider::InitStyleOption
(
	QStyleOptionSlider* pSliderOpt,
	SpanHandle m_InHandle
)const
{
	// ��������
	QSlider* pSliderPt;     // QSlider��ָ��
	
	// �������
	if (!pSliderOpt) return;

	// ָ�븳ֵ
	pSliderPt = (QSlider*)this;

	// Slider Style��ֵ
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

	// λ����ʵ��ֵ��ֵ
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
�������� HandleMousePress

���� ��  �����������¼�

��ڲ��� ��
	QPoint& PointPress ��
	QStyle::SubControl& m_SubControl �� 
	int iValue ��
	SpanHandle m_InHandle ��

��������ֵ ��

****************************************************************/
void TripleSlider::HandleMousePress
(
	QPoint& PointPress,
	QStyle::SubControl& m_SubControl,
	int iValue,
	SpanHandle m_InHandle
)
{
	// ��������
	QStyleOptionSlider SliderOpt;
	QStyle::SubControl CtrlOldControl;
	QRect m_Rect;
	QSlider* pSliderPt;					// QSlider��ָ��

	// Slider Style ����
	InitStyleOption(&SliderOpt, m_InHandle);
	SliderOpt.subControls = QStyle::SC_SliderHandle;

	// ָ�븳ֵ
	pSliderPt = (QSlider*)this;

	// �ж��Ƿ�������ȷ���Ļ�������
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

	// ������������Ļ���
	if (m_SubControl != CtrlOldControl)
		pSliderPt->update(m_Rect);
}

/**************************************************************
�������� Pick

���� ��  

��ڲ��� ��
	const QPoint& m_Point ��

��������ֵ ��

****************************************************************/
int TripleSlider::Pick(const QPoint& m_Point) const
{
	return ((this->orientation() == Qt::Horizontal) ? m_Point.x() : m_Point.y());
}

/**************************************************************
�������� PixelPosToRangeValue

���� ��  ����λ��ת����ʵ������

��ڲ��� ��
	int iPos �� ����λ��

��������ֵ ��
	int iVal :  ʵ������
****************************************************************/
int TripleSlider::PixelPosToRangeValue(int iPos) const
{
	// ��������
	int iSliderMin;
	int iSliderMax;
	int iSliderLength;
	QStyleOptionSlider SliderStyleOpt;
	QRect RectGroove;
	QRect RectHandle;
	QSlider* pSlider;

	// Slider Style Option��ʼ��
	initStyleOption(&SliderStyleOpt);

	// ������ֵ
	iSliderMin = 0;
	iSliderMax = 0;
	iSliderLength = 0;

	// ָ�븳ֵ
	pSlider = (QSlider*)this;

	// ��λ����
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
�������� SetLowerVal

���� ��  ��������

��ڲ��� ��
	int iVal ��			��������ֵ
	bool bIsUpdate ��   �Ƿ����ͼ�����

��������ֵ ��

****************************************************************/
void TripleSlider::SetLowerVal
(
	int iVal, 
	bool bIsUpdate
)
{
	// ��������
	int iUpdateMid;
	float fRatio;

	// ��μ��
	if ((iVal < this->minimum()) || (iVal > iHandleVal[UpperCtrl] - 10)) return;

	// ���ޱ�������
	iUpdateMid = iHandleVal[MidCtrl];
	fRatio = (float)(iUpdateMid - iHandleVal[LowCtrl]) 
		/ (float)(iHandleVal[UpperCtrl] - iHandleVal[LowCtrl]);
	
	// ��������
	iHandleVal[LowCtrl] = iVal;

	// ���޵���
	// �������ޱ�������
	iUpdateMid = (int)round((float)iHandleVal[LowCtrl]
		+ fRatio * (float)(iHandleVal[UpperCtrl] - iHandleVal[LowCtrl]));
	SetMidVal(iUpdateMid, false);

	// �ؼ��ػ�
	if (true == bIsUpdate) update();
}

/**************************************************************
�������� SetMidVal

���� ��  ��������

��ڲ��� ��
	int iVal ��			��������ֵ
	bool bIsUpdate ��   �Ƿ����ͼ�����

��������ֵ ��

****************************************************************/
void TripleSlider::SetMidVal
(
	int iVal,
	bool bIsUpdate
)
{
	// ��μ��
	if ((iVal <= iHandleVal[LowCtrl]) || (iVal >= iHandleVal[UpperCtrl])) return;

	// ��������
	iHandleVal[MidCtrl] = iVal;

	// �ؼ��ػ�
	if (true == bIsUpdate) update();
}

/**************************************************************
�������� SetUpperVal

���� ��  ��������

��ڲ��� ��
	int iVal ��			��������ֵ
	bool bIsUpdate ��   �Ƿ����ͼ�����

��������ֵ ��

****************************************************************/
void TripleSlider::SetUpperVal
(
	int iVal,
	bool bIsUpdate
)
{
	// ��������
	int iUpdateMid;
	float fRatio;

	// ��μ��
	if ((iVal > this->maximum()) || (iVal < iHandleVal[LowCtrl] + 10)) return;

	// ���ޱ�������
	iUpdateMid = iHandleVal[MidCtrl];
	fRatio = (float)(iUpdateMid - iHandleVal[LowCtrl]) 
		/ (float)(iHandleVal[UpperCtrl] - iHandleVal[LowCtrl]);

	// ��������
	iHandleVal[UpperCtrl] = iVal;

	// ���޵���
	// �������ޱ�������
	iUpdateMid = (int)round((float)iHandleVal[LowCtrl] 
		+ fRatio * (float)(iHandleVal[UpperCtrl] - iHandleVal[LowCtrl]));
	SetMidVal(iUpdateMid, false);

	// �ؼ��ػ�
	if (true == bIsUpdate) update();
}

/**************************************************************
�������� iGetLowerVal

���� ��  ��ȡ����

��ڲ��� ��

��������ֵ ��

****************************************************************/
int TripleSlider::iGetLowerVal()
{
	return iHandleVal[0];
}

/**************************************************************
�������� iGetMidVal

���� ��  ��ȡ����

��ڲ��� ��

��������ֵ ��

****************************************************************/
int TripleSlider::iGetMidVal()
{
	return iHandleVal[1];
}

/**************************************************************
�������� iGetMidVal

���� ��  ��ȡ����

��ڲ��� ��

��������ֵ ��

****************************************************************/
int TripleSlider::iGetUpperVal()
{
	return iHandleVal[2];
}
