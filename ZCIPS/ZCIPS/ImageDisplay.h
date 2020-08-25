#pragma once

#include <QSharedPointer>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "ImageInstance.h"
#include "ImageMark.h"
#include "ImageWidget.h"


// ͼ�񳡾���
class ImageScene : public QGraphicsScene
{
	Q_OBJECT

// ����
private:

// ����
	QPointF StartPoint, EndPoint;             //������ʾ������ͼԪ�������յ�
	QPointF MoveStartPoint;                   //��ѡ��ͼԪ�ƶ�ʱ�����λ��
	bool mbDrawItem = false;                  //�ж��Ƿ��Ѵ���ͼԪ
	bool bSelStartPoint, bSelEndPoint, bSelItem, bSelRightPoint, bSelLeftPoint, bSelLeftLine, bSelTopLine, bSelRightLine, bSelBottomLine, bSelPoint;//�ж�ѡ��ͼԪ��ͼԪλ��
	void GetSelItem(QPointF SelPoint);        //��õ�ǰѡ��ͼԪ
	void CancelItemSelState();               //ȡ��ѡ��ͼԪ��ѡ��״̬��ʹScence�б���ֻ��һ��ͼԪѡ��
	void SetCursorType(QPointF SelPoint);                     //���������״ 
	QVector <QPointF>PolygonPoints;           //���ڴ��QGraphicsPolygonItem�ĵ�
	QGraphicsPolygonItem *CurrentPolygonItem;
	int PolygonSelPos;                       //������ʾ��ǰѡ��QGraphicsPolygonItem������
	bool PointIsOnline(QPointF SelPoint, QPointF SegmentStart, QPointF SegmentEnd);//�жϵ�ǰ���Ƿ����߶���
	float PointsDistance(QPointF SegmentStart, QPointF SegmentEnd);
	float PointToLineDis(QPointF SelPoint, QPointF SegmentStart, QPointF SegmentEnd);//�㵽�߶εľ���
	QPolygonF DeleteSameData(QPolygonF polygon); //ɾ������������ڵ���ͬ�ĵ�
protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	// ����
public:
	ImageScene(QWidget *parent);
	QGraphicsItem*CurrentSelItem;
	//QGraphicsRectItem*SelRectItem;
	int iItemsCount;
	ImageMouseType mouseType;
signals:
	void EditItems(QGraphicsItem*AftEditItem);
	void AddNewItem(QGraphicsItem*AddNewItem);
	void OpenPopmenu(QList<QGraphicsItem*>itemlist);
};


// ͼ����ʾ��
class ImageDisplay
{
// ����
public:
	// ��ͼ��ʵ���е�ͼ����������
	// ת��Ϊͼ����ʾ��Widget
	int InstanceToDisplay
	(
		QSharedPointer<ImageInstance> pImgIns,
		ImageWidget* pSubWidget
	);

	// ����ͼ������
	int LoadImageData
	(
		QSharedPointer<float> pfData,
		ImageScene *pImageScene,
		int iWidth,
		int iHeight
	);
	QGraphicsItem* ImageMarkToItem(MarksBase *mark);
};


//ͼԪ���Ȳ���
class QImageRuler : public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem);
public:
	QImageRuler(QObject *parent = nullptr);
	QImageRuler(QPointF StartP, QPointF EndP, QGraphicsItem *parent = nullptr);
	~QImageRuler();

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual QRectF boundingRect() const;
	void setRulerItem(QPointF StartP, QPointF EndP);
	QPointF StartPoint, EndPoint;
	float fHPixProportion, fVPixProportion;  //���г��Ȳ���ʱ�ĳ������
	enum
	{
		Type = UserType + 1
	};
	int type()const
	{
		return Type;
	}
	void setPen(QPen pen);
	QPen pen();
protected:
	//void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	//void mousePressEvent(QGraphicsSceneMouseEvent *event);
	//void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
	QPointF StartPoint1, EndPoint1, BasePoint;
	int m_width, m_height;
	QString m_string;   //��������
	QPointF GetTextPoint(QPointF point, int width, int height, float &angle); // ������������λ��
	float k;    //����б��
	QPen Drawpen;
};

/******************************************************************************************************/



