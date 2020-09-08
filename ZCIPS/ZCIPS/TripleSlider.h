#pragma once

#include <QStyle>
#include <QSlider>
#include <QStylePainter>
#include <QStyleOptionSlider>
#include <QAbstractSlider>

/******************************** ö������ ***************************************/
// ����
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

// ����
private:
	SpanHandle m_SpanHandle;
	int iHandleVal[3];	// 0 -- slider���ޣ�ʵ��ֵ��
			            // 1 -- slider��ֵ��ʵ��ֵ��
						// 2 -- slider���ޣ�ʵ��ֵ��
	int iOffset;		// slider�л�����slider�ؼ���Ե��ƫ��ֵ�����أ�
	int iPosition;		
	SpanHandle LastHandlePressed;
	SpanHandle MainHandleControl;
	QStyle::SubControl CtrlPressed[3];   // 0 -- Lower���޻���
	                                     // 1 -- Mid���޻���
										 // 2 -- Mid���޻���

// ��Ӧ������
public:

	// ��ʼ��
	TripleSlider
	(
		Qt::Orientation orientation,
		QWidget* parent = nullptr
	);

	// ������¼�����
	virtual void mousePressEvent(QMouseEvent* event);

	// ����ƶ��¼�����
	virtual void mouseMoveEvent(QMouseEvent* event);

	// ����ͷ��¼�����
	virtual void mouseReleaseEvent(QMouseEvent* event);

	// �ػ��¼�����
	virtual void paintEvent(QPaintEvent* event);

// �ź�
signals:
	// ����ֵ�ı�
	void LowerValueChanged();

	// ����ֵ�ı�
	void MidValueChanged();

	// ����ֵ�ı�
	void UpperValueChanged();

// ����
public:
	// ���ƻ���
	void DrawHandle
	(
		QStylePainter* painter, 
		SpanHandle m_InHandle
	) const;

	// ��ʼ��Slider Style
	void InitStyleOption
	(
		QStyleOptionSlider* pSliderOpt, 
		SpanHandle m_InHandle = UpperHandle
	) const;

	// �����������¼�
	void HandleMousePress
	(
		QPoint& PointPress,
		QStyle::SubControl& m_SubControl,
		int iValue,
		SpanHandle m_InHandle
	);

	// 
	int Pick(const QPoint& m_Point) const;

	// ����ֵת����ʵ��ֵ
	int PixelPosToRangeValue(int iPos) const;
	
	// ��������
	void SetLowerVal
	(
		int iVal,
		bool bIsUpdate
	);

	// ��������
	void SetMidVal
	(
		int iVal,
		bool bIsUpdate
	);

	// ��������
	void SetUpperVal
	(
		int iVal,
		bool bIsUpdate
	);

	// ��ȡ����
	int iGetLowerVal();

	// ��ȡ����
	int iGetMidVal();

	// ��ȡ����
	int iGetUpperVal();

};