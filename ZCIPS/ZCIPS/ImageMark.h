#pragma once
#include <QString>
#include <QPointF>
#include <QRectF>
#include <QList>
#include <QPolygonF>
#include <QPen>
/***************************** ö������ ******************************/
//�������
enum ImageMarkType
{
	MarkType_rect = 0,    //����
	MarkType_line,        //ֱ��
	MarkType_ellipse,    //��Բ
	MarkType_polygon,    //�Զ�����״
	MarkType_Ruler,       //���Ȳ���
	MarkType_group       //��Ҫ�����Զ�ȱ�ݱ��
};
/************************** ��ǰ���Ҫ���ı������ ö������********************/
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
/******************************** ����� **************************/
//����
class MarksBase
{
public:
	MarksBase();
	int type;
	QString tooltip;

};
/****************************************************/
//ֱ��
class MarkLine: public MarksBase
{
public:
	MarkLine();
	//����
public:

	int type;     //�������
	QString tooltip;
	int MarkID;
	QPointF StrartPoint;  //��ʼ��
	QPointF EndPoint;     //��ֹ��
	QPen pen;
};
/*******************************************************/
//����
class MarkRect :public MarksBase
{
public:
	MarkRect();
	//����
public:
	int type;
	QString tooltip;
	int MarkID;
	QRectF rect;
	QPen pen;
};
/******************************************************/
//��Բ
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
//�Զ�����״
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
//���Ȳ���
class MarkRuler :public MarksBase
{
public:
	MarkRuler();
	~MarkRuler();

private:
public:
	int type;     //�������
	QString tooltip;
	int MarkID;
	QPointF StrartPoint;  //��ʼ��
	QPointF EndPoint;     //��ֹ��
	float fWPixPropotion;
	float fHPixPropotion;
	QPen pen;
};


/************************************ �� ***************************************/
class ImageMark
{
public:
	QList< MarksBase* > MarkList;  //����б�
	QList< MarksBase* > AreaList;  //����ѡ�����б�
	int ItemSumCount;
public:
	ImageMark();
	void MarkListAppend(MarksBase *mark);  //��ӱ��
	void MarkListEdit(MarksBase *mark);    //�༭���
};