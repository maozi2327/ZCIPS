#include "ImageMark.h"


/*********************** 标记基类 ************************/
MarksBase::MarksBase()
{

}
/********************* 标记直线 start ***************************/
MarkLine::MarkLine()
{

}
/********************* 标记直线 end ***************************/
/********************** 标记矩形 start **************************/
MarkRect::MarkRect()
{

}
/********************** 标记矩形 end **************************/
/********************** 标记椭圆 start **************************/
MarkEllipse::MarkEllipse()
{
}

MarkEllipse::~MarkEllipse()
{
}
/********************** 标记椭圆 end **************************/
/******************** 自定义形状 start *************************/
MarkPolygon::MarkPolygon()
{
}
MarkPolygon::~MarkPolygon()
{
}
/******************* 自定义形状 end **************************/
/*******************  长度测量  start **************************/
MarkRuler::MarkRuler()
{
}

MarkRuler::~MarkRuler()
{
}
/*******************  长度测量  end **************************/
/******************** 图像标记处理类 start ************************/
ImageMark::ImageMark()
{
	ItemSumCount = 0;
}
/**********************************************************
函数名称：MarkListAppend

函数定义：想列表中添加标记记录

入口参数：
       MarksBase *mark     添加的标记
 函数返回值：
 ***********************************************************/
void ImageMark::MarkListAppend(MarksBase *mark)
{
	QString strtoolTip;
	strtoolTip = mark->tooltip;
	if (strtoolTip.length() > 4 && (strtoolTip.left(4) == "区域选择"))
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
函数名称：MarkListEdit

函数定义：编辑标记列表中的元素

入口参数：
        MarksBase *mark   被编辑的元素以及修改后的值
函数返回值：
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






	if (strtoolTip.length() > 4 && (strtoolTip.left(4) == "区域选择"))
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
/************************************** 图像标记处理类 end *************************************/