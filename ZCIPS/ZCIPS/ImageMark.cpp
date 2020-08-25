#include "ImageMark.h"


/*********************** ��ǻ��� ************************/
MarksBase::MarksBase()
{

}
/********************* ���ֱ�� start ***************************/
MarkLine::MarkLine()
{

}
/********************* ���ֱ�� end ***************************/
/********************** ��Ǿ��� start **************************/
MarkRect::MarkRect()
{

}
/********************** ��Ǿ��� end **************************/
/********************** �����Բ start **************************/
MarkEllipse::MarkEllipse()
{
}

MarkEllipse::~MarkEllipse()
{
}
/********************** �����Բ end **************************/
/******************** �Զ�����״ start *************************/
MarkPolygon::MarkPolygon()
{
}
MarkPolygon::~MarkPolygon()
{
}
/******************* �Զ�����״ end **************************/
/*******************  ���Ȳ���  start **************************/
MarkRuler::MarkRuler()
{
}

MarkRuler::~MarkRuler()
{
}
/*******************  ���Ȳ���  end **************************/
/******************** ͼ���Ǵ����� start ************************/
ImageMark::ImageMark()
{
	ItemSumCount = 0;
}
/**********************************************************
�������ƣ�MarkListAppend

�������壺���б�����ӱ�Ǽ�¼

��ڲ�����
       MarksBase *mark     ��ӵı��
 ��������ֵ��
 ***********************************************************/
void ImageMark::MarkListAppend(MarksBase *mark)
{
	QString strtoolTip;
	strtoolTip = mark->tooltip;
	if (strtoolTip.length() > 4 && (strtoolTip.left(4) == "����ѡ��"))
	{
		for (int i = 0; i < AreaList.count(); i++)
		{
			if (strtoolTip == AreaList.at(i)->tooltip)
			{
				return;
			}
		}
		AreaList.append(mark);
		ItemSumCount++;
	}
	else
	{
		for (int i = 0; i < MarkList.count(); i++)
		{
			if (strtoolTip == MarkList.at(i)->tooltip)
			{
				return;
			}
		}
		MarkList.append(mark);
		ItemSumCount++;
	}
}
/**************************************************************
�������ƣ�MarkListEdit

�������壺�༭����б��е�Ԫ��

��ڲ�����
        MarksBase *mark   ���༭��Ԫ���Լ��޸ĺ��ֵ
��������ֵ��
***************************************************************/
void ImageMark::MarkListEdit(MarksBase *mark)
{
	QString strtoolTip = mark->tooltip;
	int x, y, w, h, ty;
	QPointF ppoint;
	QString strtool;
	QPolygonF polygon;
	bool bclose;
	for (int i = 0; i < MarkList.count(); i++)
	{
		if(MarkList[i]->type == MarkType_polygon)
		{ 
		polygon = ((MarkPolygon*)MarkList[i])->polygon;
		bclose = polygon.isClosed();
		for (int j = 0; j < polygon.count(); j++)
		{
			ppoint = polygon.at(j);
		}
		}

	 //   x = ((MarkEllipse*)MarkList[i])->rect.x();
		//y = ((MarkEllipse*)MarkList[i])->rect.y();
		//w = ((MarkEllipse*)MarkList[i])->rect.width();
		//h = ((MarkEllipse*)MarkList[i])->rect.height();
		//ty = ((MarkEllipse*)MarkList[i])->type;
		//strtool = ((MarkEllipse*)MarkList[i])->tooltip;
	}






	if (strtoolTip.length() > 4 && (strtoolTip.left(4) == "����ѡ��"))
	{
		for (int i = 0; i < AreaList.count(); i++)
		{
			if (strtoolTip == AreaList.at(i)->tooltip)
			{
				switch (mark->type)
				{
				case 0:
				{
					((MarkRect*)AreaList[i])->rect = ((MarkRect*)mark)->rect;
				}
				break;
				default:
					break;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < MarkList.count(); i++)
		{
			if (strtoolTip == MarkList.at(i)->tooltip)
			{
				switch (mark->type)
				{
				case MarkType_rect:
				{
					((MarkRect*)MarkList[i])->rect = ((MarkRect*)mark)->rect; 
				}
				break;
				case MarkType_ellipse:
				{
					((MarkEllipse*)MarkList[i])->rect = ((MarkEllipse*)mark)->rect;
				}
				break;
				case MarkType_polygon:
				{
					((MarkPolygon*)MarkList[i])->polygon = ((MarkPolygon*)mark)->polygon;
				}
				break;
				default:
					break;
				}
			}
		}
	}

	for (int i = 0; i < MarkList.count(); i++)
	{
		if (MarkList[i]->type == MarkType_polygon)
		{ 
		polygon = ((MarkPolygon*)MarkList[i])->polygon;
		bclose = polygon.isClosed();
		for (int j = 0; j < polygon.count(); j++)
		{
			ppoint = polygon.at(j);
		}
		}
		//x = ((MarkEllipse*)MarkList[i])->rect.x();
		//y = ((MarkEllipse*)MarkList[i])->rect.y();
		//w = ((MarkEllipse*)MarkList[i])->rect.width();
		//h = ((MarkEllipse*)MarkList[i])->rect.height();
		//ty = ((MarkEllipse*)MarkList[i])->type;
		//strtool = ((MarkEllipse*)MarkList[i])->tooltip;
	}


}
/************************************** ͼ���Ǵ����� end *************************************/