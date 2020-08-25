#pragma once

#include <QSharedPointer>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "ImageInstance.h"
#include "ImageMark.h"
#include "ImageWidget.h"


// 图像场景类
class ImageScene : public QGraphicsScene
{
	Q_OBJECT

// 参数
private:

// 函数
	QPointF StartPoint, EndPoint;             //用来表示所绘制图元的起点和终点
	QPointF MoveStartPoint;                   //当选中图元移动时的起点位置
	bool mbDrawItem = false;                  //判断是否已存在图元
	bool bSelStartPoint, bSelEndPoint, bSelItem, bSelRightPoint, bSelLeftPoint, bSelLeftLine, bSelTopLine, bSelRightLine, bSelBottomLine, bSelPoint;//判断选中图元的图元位置
	void GetSelItem(QPointF SelPoint);        //获得当前选中图元
	void CancelItemSelState();               //取消选中图元的选中状态，使Scence中保持只有一个图元选中
	void SetCursorType(QPointF SelPoint);                     //设置鼠标形状 
	QVector <QPointF>PolygonPoints;           //用于存放QGraphicsPolygonItem的点
	QGraphicsPolygonItem *CurrentPolygonItem;
	int PolygonSelPos;                       //用来表示当前选中QGraphicsPolygonItem点的序号
	bool PointIsOnline(QPointF SelPoint, QPointF SegmentStart, QPointF SegmentEnd);//判断当前点是否在线段上
	float PointsDistance(QPointF SegmentStart, QPointF SegmentEnd);
	float PointToLineDis(QPointF SelPoint, QPointF SegmentStart, QPointF SegmentEnd);//点到线段的距离
	QPolygonF DeleteSameData(QPolygonF polygon); //删除多边形中相邻的相同的点
protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	// 函数
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


// 图像显示类
class ImageDisplay
{
// 函数
public:
	// 将图像实例中的图像数据与标记
	// 转换为图像显示的Widget
	int InstanceToDisplay
	(
		QSharedPointer<ImageInstance> pImgIns,
		ImageWidget* pSubWidget
	);

	// 加载图像数据
	int LoadImageData
	(
		QSharedPointer<float> pfData,
		ImageScene *pImageScene,
		int iWidth,
		int iHeight
	);
	QGraphicsItem* ImageMarkToItem(MarksBase *mark);
};


//图元长度测量
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
	float fHPixProportion, fVPixProportion;  //进行长度测量时的长宽比例
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
	QString m_string;   //计算文字
	QPointF GetTextPoint(QPointF point, int width, int height, float &angle); // 计算文字所在位置
	float k;    //计算斜率
	QPen Drawpen;
};

/******************************************************************************************************/



