#pragma once
#include <QString>
#include <QPointF>
#include <QRectF>
#include <QList>
#include <QPolygonF>
#include <QPen>
/***************************** 枚举类型 ******************************/
//标记类型
enum ImageMarkType
{
	MarkType_rect = 0,    //矩形
	MarkType_line,        //直线
	MarkType_ellipse,    //椭圆
	MarkType_polygon,    //自定义形状
	MarkType_Ruler,       //长度测量
	MarkType_group       //主要用于自动缺陷标记
};
/************************** 当前鼠标要做的标记类型 枚举类型********************/
enum ImageMouseType
{
	Mouse_Rect = 0,
	Mouse_Ellipse,
	Mouse_Polygon,
	Mouse_Ruler,
	Mouse_Line,
	Mouse_RectArea,
	Mouse_EllipseArea,
	Mouse_PolyArea,
	Mouse_Default
};
/******************************** 标记类 **************************/
//基类
class MarksBase
{
public:
	MarksBase();
	int type;
	QString tooltip;

};
/****************************************************/
//直线
class MarkLine: public MarksBase
{
public:
	MarkLine();
	//参数
public:

	int type;     //标记类型
	QString tooltip;
	int MarkID;
	QPointF StrartPoint;  //起始点
	QPointF EndPoint;     //终止点
	QPen pen;
};
/*******************************************************/
//矩形
class MarkRect :public MarksBase
{
public:
	MarkRect();
	//参数
public:
	int type;
	QString tooltip;
	int MarkID;
	QRectF rect;
	QPen pen;
};
/******************************************************/
//椭圆
class MarkEllipse :public MarksBase
{
public:
	MarkEllipse();
	~MarkEllipse();

private:
public:
	int type;
	QString tooltip;
	int MarkID;
	QRectF rect;
	QPen pen;
};
/**********************************************************/
//自定义形状
class MarkPolygon :public MarksBase
{
public:
	MarkPolygon();
	~MarkPolygon();
private:
public:
	int type;
	QString tooltip;
	int MarkID;
	QPolygonF polygon;
	QPen pen;
};
/********************************************************************/
//长度测量
class MarkRuler :public MarksBase
{
public:
	MarkRuler();
	~MarkRuler();

private:
public:
	int type;     //标记类型
	QString tooltip;
	int MarkID;
	QPointF StrartPoint;  //起始点
	QPointF EndPoint;     //终止点
	float fWPixPropotion;
	float fHPixPropotion;
	QPen pen;
};


/************************************ 类 ***************************************/
class ImageMark
{
public:
	QList< MarksBase* > MarkList;  //标记列表
	QList< MarksBase* > AreaList;  //区域选择标记列表
	int ItemSumCount;
public:
	ImageMark();
	void MarkListAppend(MarksBase *mark);  //添加标记
	void MarkListEdit(MarksBase *mark);    //编辑标记
};