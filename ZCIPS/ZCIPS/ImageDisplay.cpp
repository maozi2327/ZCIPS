#include "ImageDisplay.h"
#include "ERRInfo.h"
#include "ImageMark.h"
/*******************************  Start : ImageWidget   ************************************/

/**************************************************************
�������� LoadImageData

���� ��  ����ͼ��������ͼ��ͼԪ

��ڲ��� ��
	QSharedPointer<float> pfData �� ͼ������
	ImageScene *pImageScene ��		����ָ��
	int iWidth ��
	int iHeight ��

��������ֵ ��
	int iRet ��    0    -- �ɹ�
				   ���� -- ʧ��
****************************************************************/
int ImageDisplay::LoadImageData
(
	QSharedPointer<float> pfData,
	ImageScene *pImageScene,
	int iWidth,
	int iHeight
)
{
	// ��������
	QPixmap m_Pixmap;
	QImage m_Image;
	QGraphicsPixmapItem *pImageItem;
	uchar *pucPtData;                       // ����ָ��
	
	// �ӳ��ߴ���
	pImageScene->setSceneRect(0, 0, iWidth, iHeight);

	// ����������QImage
	m_Image = QImage(iWidth, iHeight, QImage::Format_Grayscale8);
	pucPtData = m_Image.bits();
	for (int i = 0; i < iHeight; i++)
		for (int j = 0; j < iWidth; j++)
		{
			pucPtData[i * iWidth + j]
				= (uchar)round(pfData.get()[i * iWidth + j]);
		}

	// ����QImage��ͼ��ͼԪ
	m_Pixmap = QPixmap::fromImage(m_Image);
	pImageItem = new QGraphicsPixmapItem();
	pImageItem->setPixmap(m_Pixmap);
	
	//// ����ͼԪ������
	pImageScene->addItem(pImageItem);

	return SUCCESS;
}

/**************************************************************
�������� InstanceToDisplay

���� ��  ͼ��ʵ��ת��ΪWidget

��ڲ��� ��
	QSharedPointer<ImageInstance> pImgIns �� ͼ��ʵ��
	QWidget *pSubWidget ��					 ͼ����ʾ�ؼ�

��������ֵ ��
	int iRet ��    0    -- �ɹ�
				   ���� -- ʧ��
****************************************************************/
int ImageDisplay::InstanceToDisplay
(
	QSharedPointer<ImageInstance> pImgIns,
	ImageWidget* pSubWidget
)
{
	// ��������
	int iRet;
	int iWidgetHeight;
	int iWidgetWidth;
	int iImgHeight;
	int iImgWidth;
	QGraphicsView *pImageView;				// ͼ���ӳ�
	ImageScene *pImageScene;				// ͼ�񳡾�
	QGraphicsPixmapItem *pImageItem;		// ͼ��ͼԪ

	// ��μ��
	if (nullptr == pSubWidget) return ERR_INPUT;

	// ������ֵ
	pImageView = nullptr;
	pImageScene = nullptr;
	pImageItem = nullptr;

	// ��ȡͼ�����
	iWidgetHeight = pSubWidget->height();
	iWidgetWidth = pSubWidget->width();
	iImgHeight = pImgIns.get()->pImageData.get()->iHeight;
	iImgWidth = pImgIns.get()->pImageData.get()->iWidth;

	// Viewָ���ȡ���������
	pImageView = pSubWidget->findChild<QGraphicsView *>(QStringLiteral("ImageView"));
	if (pImageView == nullptr)
	{
		pImageView = new QGraphicsView(pSubWidget);
		pImageView->setObjectName(QStringLiteral("ImageView"));
		pImageView->setMouseTracking(true);
		pImageView->setRenderHint(QPainter::Antialiasing, true);
		pImageView->setGeometry(0, 0, iWidgetWidth, iWidgetHeight);
	}
	
	// Sceneָ���ȡ���������
	pImageScene = (ImageScene *)pImageView->scene();
	if (pImageScene == nullptr)
	{ 
		pImageScene = new ImageScene(pImageView);
		pImageScene->setObjectName(QStringLiteral("ImageScene"));
		pImageView->setScene(pImageScene);
	}
		
	// ͼ������ת��
	iRet = LoadImageData(
		pImgIns.get()->pImageData.get()->pfProcessingData,
		pImageScene,
		iImgWidth,
		iImgHeight);
	if (SUCCESS != iRet) return ERR_FUNC;

	// ���ת��
	QGraphicsItem*item;
	int countt = pImgIns->pImageMarks->MarkList.count();
	for (int i = 0; i < pImgIns->pImageMarks->MarkList.count(); i++)
	{
		item = ImageMarkToItem(pImgIns->pImageMarks->MarkList.at(i));
		item->setFlag(QGraphicsItem::ItemIsMovable, true);
		item->setFlag(QGraphicsItem::ItemIsSelectable, true);
		pImageScene->addItem(item);
	}
	pImageScene->iItemsCount = pImgIns->pImageMarks->ItemSumCount;
	pImageScene->mouseType = pImgIns->mouseType;

	return SUCCESS;
}
/***********************************************************************
�������ƣ�ImageMarkToItem

�������壺�������Ϣת��ΪͼԪ

��ڲ�����
		 MarksBase *mark     ��ת����ͼԪ�ı��

��������ֵ��
		 QGraphicsItem*    �����ת���ɵ�ͼԪ
*************************************************************************/
QGraphicsItem* ImageDisplay::ImageMarkToItem(MarksBase *mark)
{
	QGraphicsItem*item;
	switch (mark->type)
	{
	case MarkType_rect:
	{//����
		QGraphicsRectItem*rectItem = new QGraphicsRectItem;
		rectItem->setToolTip(mark->tooltip);
		rectItem->setRect(((MarkRect*)mark)->rect);
		rectItem->setPen(((MarkRect*)mark)->pen);
		return rectItem;
	}
	break;
	case MarkType_ellipse:
	{//��Բ
		QGraphicsEllipseItem*ellipseItem = new QGraphicsEllipseItem;
		ellipseItem->setToolTip(mark->tooltip);
		ellipseItem->setRect(((MarkEllipse*)mark)->rect);
		ellipseItem->setPen(((MarkEllipse*)mark)->pen);
		return ellipseItem;
	}
	case MarkType_polygon:
	{//�����
		QGraphicsPolygonItem*polygonItem = new QGraphicsPolygonItem;
		polygonItem->setToolTip(mark->tooltip);
		polygonItem->setPolygon(((MarkPolygon*)mark)->polygon);
		polygonItem->setPen(((MarkPolygon*)mark)->pen);
		return polygonItem;
	}
	break;
	case MarkType_Ruler:
	{//���Ȳ���
		QImageRuler*rulerItem = new QImageRuler;
		rulerItem->setToolTip(mark->tooltip);
		rulerItem->fHPixProportion = ((MarkRuler*)mark)->fHPixPropotion;
		rulerItem->fVPixProportion = ((MarkRuler*)mark)->fWPixPropotion;
		rulerItem->setPen(((MarkRuler*)mark)->pen);
		rulerItem->setRulerItem(((MarkRuler*)mark)->StrartPoint, ((MarkRuler*)mark)->EndPoint);
		return rulerItem;
	}
	break;
	default:
		break;
	}
}

/*******************************  End : ImageWidget   ************************************/




/*******************************  Start : ImageScene   ************************************/

/**************************************************************
�������� ImageScene

���� ��  ���캯��

��ڲ��� ��
	QWidget *parent : ��ָ��

��������ֵ ��

****************************************************************/
ImageScene::ImageScene(QWidget *parent = nullptr) :
	QGraphicsScene(parent)
{
	bSelStartPoint = false;
	bSelEndPoint = false;
	bSelItem = false;
	bSelRightPoint = false;
	bSelLeftPoint = false;
	bSelLeftLine = false;
	bSelTopLine = false;
	bSelRightLine = false;
	bSelBottomLine = false;
	bSelPoint = false;
	CurrentSelItem = nullptr;
	iItemsCount = 0;
	mouseType = Mouse_Default;
}
/**************************************************************
�������ƣ�mousePressEvent

�������壺ImageScene��������¼�

��ڲ�����
		 QGraphicsSceneMouseEvent *event     QGraphicsScene����¼�

��������ֵ��
*************************************************************/
void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	StartPoint = event->scenePos();
	EndPoint = StartPoint + QPointF(1, 1);

	if (event->button() == Qt::LeftButton)
	{//��������
		GetSelItem(StartPoint);
		MoveStartPoint = event->scenePos();
		QString strTooltip;
		switch (mouseType)
		{
		case Mouse_Rect:
		{
			if (CurrentSelItem == nullptr)
			{
				QPen pen;
				pen.setColor(QColor(0, 255, 0));
				QGraphicsRectItem*SelRectItem = this->addRect(StartPoint.x(), StartPoint.y(), 1, 1);
				SelRectItem->setFlag(QGraphicsItem::ItemIsMovable, true);
				SelRectItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
				SelRectItem->setPen(pen);
				CancelItemSelState();
				SelRectItem->setSelected(true);
				CurrentSelItem = nullptr;
				CurrentSelItem = SelRectItem;
				SelRectItem->setToolTip(QStringLiteral("���α��") + QString::number(iItemsCount));
			}
			mbDrawItem = true;
		}
		break;
		case Mouse_Ellipse:
		{
			if (CurrentSelItem == nullptr)
			{
				QPen pen;
				pen.setColor(QColor(0, 0, 255));
				QGraphicsEllipseItem*SelEllipseItem = this->addEllipse(StartPoint.x(), StartPoint.y(), 1, 1);
				SelEllipseItem->setFlag(QGraphicsItem::ItemIsMovable, true);
				SelEllipseItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
				SelEllipseItem->setPen(pen);
				CancelItemSelState();
				SelEllipseItem->setSelected(true);
				CurrentSelItem = nullptr;
				CurrentSelItem = SelEllipseItem;
				SelEllipseItem->setToolTip(QStringLiteral("��Բ���") + QString::number(iItemsCount));
			}
			mbDrawItem = true;
		}
		break;
		case Mouse_Polygon:
		{
			if (CurrentSelItem == nullptr)
			{
				QPolygonF polygon = QPolygonF(PolygonPoints);
				if ((PolygonPoints.count() > 1) && (PolygonPoints.at(0) == PolygonPoints.at(PolygonPoints.count() - 1)))
				{
					QVector <QPointF> pNullVector;
					PolygonPoints.swap(pNullVector);
					CurrentPolygonItem = nullptr;
				}
				if (PolygonPoints.count() == 0)
				{
					QPen pen;
					pen.setColor(QColor(255, 0, 0));
					PolygonPoints.push_back(StartPoint);
					PolygonPoints.push_back(EndPoint);
					CurrentPolygonItem = this->addPolygon(QPolygonF(PolygonPoints));
					CurrentPolygonItem->setFlag(QGraphicsItem::ItemIsMovable, true);
					CurrentPolygonItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
					CurrentPolygonItem->setPen(pen);
					CancelItemSelState();
					CurrentPolygonItem->setSelected(true);
					CurrentSelItem = nullptr;
					CurrentSelItem = CurrentPolygonItem;
					CurrentPolygonItem->setToolTip(QStringLiteral("�Զ�����״���") + QString::number(iItemsCount));
				}
				else
				{
					PolygonPoints[PolygonPoints.count() - 1] = event->scenePos();
					float dist = PointsDistance(PolygonPoints[PolygonPoints.count() - 1], PolygonPoints[PolygonPoints.count() - 2]);
					if (dist > 3)
					{
						PolygonPoints.push_back(event->scenePos());
					}
					CurrentPolygonItem->setPolygon(QPolygonF(PolygonPoints));
					CurrentSelItem = nullptr;
					CurrentSelItem = CurrentPolygonItem;
				}
			}
			mbDrawItem = true;
		}
		break;
		case Mouse_Ruler:
		{
			if (CurrentSelItem == nullptr)
			{
				QPen pen;
				pen.setColor(QColor(0, 0, 255));
				QImageRuler*SelRulerItem = new QImageRuler;
				SelRulerItem->setRulerItem(StartPoint, EndPoint);
				this->addItem(SelRulerItem);
				SelRulerItem->setFlag(QGraphicsItem::ItemIsMovable, true);
				SelRulerItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
				SelRulerItem->setPen(pen);
				CancelItemSelState();
				SelRulerItem->setSelected(true);
				CurrentSelItem = nullptr;
				CurrentSelItem = SelRulerItem;
				SelRulerItem->setToolTip(QStringLiteral("���Ȳ���") + QString::number(iItemsCount));
			}
			mbDrawItem = true;
		}
		break;
		case Mouse_Line:
			break;
		case Mouse_RectArea:
			break;
		case Mouse_EllipseArea:
			break;
		case Mouse_PolyArea:
			break;
		case Mouse_Default:
		{
			QTransform Transform;
			QGraphicsItem *item = itemAt(event->scenePos(), Transform);
			if ((item == 0) || (item->type() == QGraphicsPixmapItem::Type))
			{
				CancelItemSelState();
			}
			if (CurrentSelItem != nullptr)
			{
				mbDrawItem = true;
			}
		}
		break;
		default:
			break;
		}
	}
}
/**********************************************************************
�������ƣ�mouseMoveEvent

�������壺ImageScene����ƶ��¼�

��ڲ�����
		 QGraphicsSceneMouseEvent *event     QGraphicsScene����¼�

��������ֵ��
*********************************************************************/
void ImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QPointF MovePoint = event->scenePos();
	QPointF rightPoint, leftPoint;
	SetCursorType(MovePoint);
	if (mbDrawItem)
	{
		switch (CurrentSelItem->type())
		{
		case QGraphicsRectItem::Type:
		{
			QGraphicsRectItem*SelRectItem = qgraphicsitem_cast<QGraphicsRectItem*>(CurrentSelItem);
			if (bSelEndPoint)
			{
				if (MovePoint.x() == StartPoint.x())
				{
					MovePoint.setX(StartPoint.x() + 1);
				}
				if (MovePoint.y() == StartPoint.y())
				{
					MovePoint.setY(StartPoint.y() + 1);
				}
				EndPoint = MovePoint;
				SelRectItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelRightPoint)
			{
				leftPoint.setX(StartPoint.x());
				leftPoint.setY(EndPoint.y());
				rightPoint.setX(EndPoint.x());
				rightPoint.setY(StartPoint.y());
				if (MovePoint.x() == leftPoint.x())
				{
					MovePoint.setX(leftPoint.x() + 1);
				}
				if (MovePoint.y() == leftPoint.y())
				{
					MovePoint.setY(leftPoint.y() - 1);
				}
				StartPoint.setX(leftPoint.x());
				StartPoint.setY(MovePoint.y());
				EndPoint.setX(MovePoint.x());
				EndPoint.setY(leftPoint.y());
				SelRectItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelLeftPoint)
			{
				rightPoint.setX(EndPoint.x());
				rightPoint.setY(StartPoint.y());
				if (MovePoint.x() == rightPoint.x())
				{
					MovePoint.setX(rightPoint.x() - 1);
				}
				if (MovePoint.y() == rightPoint.y())
				{
					MovePoint.setY(rightPoint.y() + 1);
				}
				StartPoint.setX(MovePoint.x());
				StartPoint.setY(rightPoint.y());
				EndPoint.setX(rightPoint.x());
				EndPoint.setY(MovePoint.y());
				SelRectItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelStartPoint)
			{
				if (MovePoint.x() == EndPoint.x())
				{
					MovePoint.setX(EndPoint.x() - 1);
				}
				if (MovePoint.y() == EndPoint.y())
				{
					MovePoint.setY(EndPoint.y() - 1);
				}
				StartPoint = MovePoint;
				SelRectItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelRightLine)
			{
				if (MovePoint.x() == StartPoint.x())
				{
					MovePoint.setX(StartPoint.x() + 1);
				}
				EndPoint.setX(MovePoint.x());
				SelRectItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelBottomLine)
			{
				if (MovePoint.y() == StartPoint.y())
				{
					MovePoint.setY(StartPoint.y() + 1);
				}
				EndPoint.setY(MovePoint.y());
				SelRectItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelLeftLine)
			{
				if (MovePoint.x() == EndPoint.x())
				{
					MovePoint.setX(EndPoint.x() - 1);
				}
				StartPoint.setX(MovePoint.x());
				SelRectItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelTopLine)
			{
				if (MovePoint.y() == EndPoint.y())
				{
					MovePoint.setY(EndPoint.y() - 1);
				}
				StartPoint.setY(MovePoint.y());
				SelRectItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelItem)
			{
				StartPoint = StartPoint + MovePoint - MoveStartPoint;
				EndPoint = EndPoint + MovePoint - MoveStartPoint;
				MoveStartPoint = MovePoint;
				SelRectItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else
			{
				EndPoint = event->scenePos();
				int iwidth, iheight;
				if (StartPoint.x() > EndPoint.x())
				{
					iwidth = StartPoint.x() - EndPoint.x();
					StartPoint.setX(EndPoint.x());
				}
				else
				{
					iwidth = EndPoint.x() - StartPoint.x();
				}
				if (StartPoint.y() > EndPoint.y())
				{
					iheight = StartPoint.y() - EndPoint.y();
					StartPoint.setY(EndPoint.y());
				}
				else
				{
					iheight = EndPoint.y() - StartPoint.y();
				}
				SelRectItem->setRect(StartPoint.x(), StartPoint.y(), iwidth, iheight);
			}
		}
		break;
		case QGraphicsEllipseItem::Type:
		{
			QGraphicsEllipseItem*SelEllipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(CurrentSelItem);
			if (bSelEndPoint)
			{
				if (MovePoint.x() == StartPoint.x())
				{
					MovePoint.setX(StartPoint.x() + 1);
				}
				if (MovePoint.y() == StartPoint.y())
				{
					MovePoint.setY(StartPoint.y() + 1);
				}
				EndPoint = MovePoint;
				SelEllipseItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelRightPoint)
			{
				leftPoint.setX(StartPoint.x());
				leftPoint.setY(EndPoint.y());
				rightPoint.setX(EndPoint.x());
				rightPoint.setY(StartPoint.y());
				if (MovePoint.x() == leftPoint.x())
				{
					MovePoint.setX(leftPoint.x() + 1);
				}
				if (MovePoint.y() == leftPoint.y())
				{
					MovePoint.setY(leftPoint.y() - 1);
				}
				StartPoint.setX(leftPoint.x());
				StartPoint.setY(MovePoint.y());
				EndPoint.setX(MovePoint.x());
				EndPoint.setY(leftPoint.y());
				SelEllipseItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelLeftPoint)
			{
				rightPoint.setX(EndPoint.x());
				rightPoint.setY(StartPoint.y());
				if (MovePoint.x() == rightPoint.x())
				{
					MovePoint.setX(rightPoint.x() - 1);
				}
				if (MovePoint.y() == rightPoint.y())
				{
					MovePoint.setY(rightPoint.y() + 1);
				}
				StartPoint.setX(MovePoint.x());
				StartPoint.setY(rightPoint.y());
				EndPoint.setX(rightPoint.x());
				EndPoint.setY(MovePoint.y());
				SelEllipseItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelStartPoint)
			{
				if (MovePoint.x() == EndPoint.x())
				{
					MovePoint.setX(EndPoint.x() - 1);
				}
				if (MovePoint.y() == EndPoint.y())
				{
					MovePoint.setY(EndPoint.y() - 1);
				}
				StartPoint = MovePoint;
				SelEllipseItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelRightLine)
			{
				if (MovePoint.x() == StartPoint.x())
				{
					MovePoint.setX(StartPoint.x() + 1);
				}
				EndPoint.setX(MovePoint.x());
				SelEllipseItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelBottomLine)
			{
				if (MovePoint.y() == StartPoint.y())
				{
					MovePoint.setY(StartPoint.y() + 1);
				}
				EndPoint.setY(MovePoint.y());
				SelEllipseItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelLeftLine)
			{
				if (MovePoint.x() == EndPoint.x())
				{
					MovePoint.setX(EndPoint.x() - 1);
				}
				StartPoint.setX(MovePoint.x());
				SelEllipseItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelTopLine)
			{
				if (MovePoint.y() == EndPoint.y())
				{
					MovePoint.setY(EndPoint.y() - 1);
				}
				StartPoint.setY(MovePoint.y());
				SelEllipseItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else if (bSelItem)
			{
				StartPoint = StartPoint + MovePoint - MoveStartPoint;
				EndPoint = EndPoint + MovePoint - MoveStartPoint;
				MoveStartPoint = MovePoint;
				SelEllipseItem->setRect(StartPoint.x(), StartPoint.y(), EndPoint.x() - StartPoint.x(), EndPoint.y() - StartPoint.y());
			}
			else
			{
				EndPoint = event->scenePos();
				int iwidth, iheight;
				if (StartPoint.x() > EndPoint.x())
				{
					iwidth = StartPoint.x() - EndPoint.x();
					StartPoint.setX(EndPoint.x());
				}
				else
				{
					iwidth = EndPoint.x() - StartPoint.x();
				}
				if (StartPoint.y() > EndPoint.y())
				{
					iheight = StartPoint.y() - EndPoint.y();
					StartPoint.setY(EndPoint.y());
				}
				else
				{
					iheight = EndPoint.y() - StartPoint.y();
				}
				SelEllipseItem->setRect(StartPoint.x(), StartPoint.y(), iwidth, iheight);
			}
		}
		break;
		case QGraphicsPolygonItem::Type:
		{
			QGraphicsPolygonItem*SelPolygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(CurrentSelItem);
			QPolygonF polygon;
			if (bSelItem)
			{
				polygon = SelPolygonItem->polygon();
				for (int i = 0; i < polygon.count(); i++)
				{
					polygon[i] = polygon.at(i) + MovePoint - MoveStartPoint;
				}
				MoveStartPoint = MovePoint;
				SelPolygonItem->setPolygon(polygon);
			}
			else if (bSelPoint)
			{
				polygon = SelPolygonItem->polygon();
				EndPoint = event->scenePos();
				polygon[PolygonSelPos] = EndPoint;
				SelPolygonItem->setPolygon(polygon);
			}
			else
			{
				polygon = QPolygonF(PolygonPoints);
				if ((PolygonPoints.count() > 1) && (PolygonPoints.at(0) == PolygonPoints.at(PolygonPoints.count() - 1)))
				{

				}
				else
				{
					EndPoint = event->scenePos();
					int count = PolygonPoints.count();
					PolygonPoints[count - 1] = EndPoint;
					CurrentPolygonItem->setPolygon(QPolygonF(PolygonPoints));
				}
			}
		}
		break;
		case QImageRuler::Type:
		{
			QImageRuler*SelRulerItem = qgraphicsitem_cast<QImageRuler*>(CurrentSelItem);
			if (bSelStartPoint)
			{
				//StartPoint = SelRulerItem->EndPoint;
				EndPoint = event->scenePos();
				SelRulerItem->setRulerItem(StartPoint, EndPoint);
			}
			else if (bSelEndPoint)
			{
				EndPoint = event->scenePos();
				SelRulerItem->setRulerItem(StartPoint, EndPoint);
			}
			else if (bSelItem)
			{
				StartPoint = StartPoint + MovePoint - MoveStartPoint;
				EndPoint = EndPoint + MovePoint - MoveStartPoint;
				MoveStartPoint = MovePoint;
				SelRulerItem->setRulerItem(StartPoint, EndPoint);
			}
			else
			{
				EndPoint = event->scenePos();
				SelRulerItem->setRulerItem(StartPoint, EndPoint);
			}
			this->invalidate();
		}
		break;
		default:
			break;
		}

	}
}
/**************************************************************************
�������ƣ�mouseReleaseEvent

�������壺ImageScene����ͷ��¼�

��ڲ�����
		 QGraphicsSceneMouseEvent *event    QGraphicsSceneMouseEvent����¼�

��������ֵ��
***************************************************************************/
void ImageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		emit OpenPopmenu(selectedItems());
	}
	if (mbDrawItem)
	{
		if (bSelStartPoint || bSelEndPoint || bSelItem || bSelRightPoint || bSelLeftPoint
			|| bSelLeftLine || bSelTopLine || bSelRightLine || bSelBottomLine || bSelPoint)
		{
			bSelStartPoint = false;
			bSelEndPoint = false;
			bSelItem = false;
			bSelRightPoint = false;
			bSelLeftPoint = false;
			bSelLeftLine = false;
			bSelTopLine = false;
			bSelRightLine = false;
			bSelBottomLine = false;
			bSelPoint = false;
			emit EditItems(CurrentSelItem);
			mouseType = Mouse_Default;
		}
		else
		{
			switch (CurrentSelItem->type())
			{
			case QGraphicsRectItem::Type:
			{
				QGraphicsRectItem*item = qgraphicsitem_cast<QGraphicsRectItem*>(CurrentSelItem);
				if (item->rect().width() > 1 || item->rect().height() > 1)
				{
					iItemsCount++;
					emit AddNewItem(CurrentSelItem);
				}
				else
				{
					removeItem(CurrentSelItem);
				}
				mouseType = Mouse_Default;
			}
			break;
			case QGraphicsEllipseItem::Type:
			{
				QGraphicsEllipseItem*item = qgraphicsitem_cast<QGraphicsEllipseItem*>(CurrentSelItem);
				if (item->rect().width() > 1 || item->rect().height() > 1)
				{
					iItemsCount++;
					emit AddNewItem(CurrentSelItem);
				}
				else
				{
					removeItem(CurrentSelItem);
				}
				mouseType = Mouse_Default;
			}
			break;
			case QGraphicsPolygonItem::Type:
			{
				QPolygonF polygon = QPolygonF(PolygonPoints);
				float distance;
				if (PolygonPoints.count() > 2)
				{
					distance = PointsDistance(PolygonPoints.at(0), PolygonPoints.at(PolygonPoints.count() - 1));
					if (distance < 5)
					{
						PolygonPoints[PolygonPoints.count() - 1] = PolygonPoints.at(0);
					}
				}
				polygon = QPolygonF(PolygonPoints);
				CurrentPolygonItem->setPolygon(polygon);
				if (polygon.isClosed())
				{
					QPointF tem1, tem2;
					for (int i = 0; i < polygon.count(); i++)
					{
						tem1 = PolygonPoints.at(i);
						tem2 = polygon.at(i);
					}
					iItemsCount++;
					emit AddNewItem(CurrentSelItem);
					QVector <QPointF> pNullVector;
					PolygonPoints.swap(pNullVector);
					CurrentPolygonItem = nullptr;
					mouseType = Mouse_Default;
				}
			}
			break;
			default:
				iItemsCount++;
				emit AddNewItem(CurrentSelItem);
				mouseType = Mouse_Default;
				break;
			}
		}
		mbDrawItem = false;
	}
	else
	{
	}
}
/************************************************************************************
�������ƣ�mouseDoubleClickEvent

�������壺ImageScene���˫���¼�

��ڲ�����
		 QGraphicsSceneMouseEvent *event    QGraphicsScene����¼�

��������ֵ��
************************************************************************************/
void ImageScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (mouseType == Mouse_Polygon && (PolygonPoints.count() != 0))
	{
		//
		float dist = PointsDistance(PolygonPoints[PolygonPoints.count() - 1], PolygonPoints[PolygonPoints.count() - 2]);
		if (dist < 3)
		{
			PolygonPoints[PolygonPoints.count() - 1] = PolygonPoints.at(0);
		}
		else
		{
			PolygonPoints.push_back(PolygonPoints.at(0));
		}
		int count = items().count();
		if (PolygonPoints.count() == 3)
		{
			removeItem(CurrentSelItem);
			count = items().count();
		}
		else
		{
			iItemsCount++;
			CurrentPolygonItem->setPolygon(QPolygonF(PolygonPoints));
			CurrentSelItem = CurrentPolygonItem;
			emit AddNewItem(CurrentSelItem);
			QVector <QPointF> pNullVector;
			PolygonPoints.swap(pNullVector);
			CurrentPolygonItem = nullptr;
			mouseType = Mouse_Default;
		}
	}
}
/********************************************************************************
�������ƣ�GetSelItem

�������壺��õ�ǰ���λ����ѡ�е�ͼԪ

��ڲ�����
		 QPointF SelPoint    ��ǰ���λ��

��������ֵ��
*******************************************************************************/
void ImageScene::GetSelItem(QPointF SelPoint)
{
	CurrentSelItem = nullptr;
	QTransform Transform;
	QGraphicsItem *item = itemAt(StartPoint, Transform);
	QPointF rightPoint, leftPoint;
	float dist;
	if (item != 0 && (item->type() != QGraphicsPixmapItem::Type))
	{
		CancelItemSelState();
		CurrentSelItem = item;
		CurrentSelItem->setSelected(true);
		switch (CurrentSelItem->type())
		{
		case QGraphicsRectItem::Type:
		{
			QGraphicsRectItem*SelRectItem = qgraphicsitem_cast<QGraphicsRectItem*>(CurrentSelItem);

			StartPoint.setX(SelRectItem->rect().x());
			StartPoint.setY(SelRectItem->rect().y());
			EndPoint.setX(SelRectItem->rect().x() + SelRectItem->rect().width());
			EndPoint.setY(SelRectItem->rect().y() + SelRectItem->rect().height());
			rightPoint.setX(SelRectItem->rect().x() + SelRectItem->rect().width());
			rightPoint.setY(SelRectItem->rect().y());
			leftPoint.setX(SelRectItem->rect().x());
			leftPoint.setY(SelRectItem->rect().y() + SelRectItem->rect().height());
			if (sqrt((SelPoint.x() - EndPoint.x())*(SelPoint.x() - EndPoint.x()) + (SelPoint.y() - EndPoint.y())*(SelPoint.y() - EndPoint.y())) < 3)
			{  //ѡ�����½ǵ�
				bSelEndPoint = true;
			}
			else if (sqrt((SelPoint.x() - rightPoint.x())*(SelPoint.x() - rightPoint.x()) + (SelPoint.y() - rightPoint.y())*(SelPoint.y() - rightPoint.y())) < 3)
			{  //ѡ�����Ͻǵ�
				bSelRightPoint = true;
			}
			else if (sqrt((SelPoint.x() - leftPoint.x())*(SelPoint.x() - leftPoint.x()) + (SelPoint.y() - leftPoint.y())*(SelPoint.y() - leftPoint.y())) < 3)
			{   //ѡ�����½ǵ�
				bSelLeftPoint = true;
			}
			else if (sqrt((SelPoint.x() - StartPoint.x())*(SelPoint.x() - StartPoint.x()) + (SelPoint.y() - StartPoint.y())*(SelPoint.y() - StartPoint.y())) < 3)
			{   //ѡ�����Ͻǵ�
				bSelStartPoint = true;
			}
			else if (abs(SelPoint.x() - EndPoint.x()) < 2)
			{   //ѡ���ұ�
				bSelRightLine = true;
			}
			else if (abs(SelPoint.y() - EndPoint.y()) < 2)
			{   //ѡ���±�
				bSelBottomLine = true;
			}
			else if (abs(SelPoint.x() - StartPoint.x()) < 2)
			{   //ѡ�����
				bSelLeftLine = true;
			}
			else if (abs(SelPoint.y() - StartPoint.y()) < 2)
			{   //ѡ���ϱ�
				bSelTopLine = true;
			}
			else
			{  //ѡ��ͼԪ
				bSelItem = true;
			}
		}
		break;
		case QGraphicsEllipseItem::Type:
		{
			QGraphicsEllipseItem*SelEllipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(CurrentSelItem);
			StartPoint.setX(SelEllipseItem->rect().x());
			StartPoint.setY(SelEllipseItem->rect().y());
			EndPoint.setX(SelEllipseItem->rect().x() + SelEllipseItem->rect().width());
			EndPoint.setY(SelEllipseItem->rect().y() + SelEllipseItem->rect().height());
			rightPoint.setX(SelEllipseItem->rect().x() + SelEllipseItem->rect().width());
			rightPoint.setY(SelEllipseItem->rect().y());
			leftPoint.setX(SelEllipseItem->rect().x());
			leftPoint.setY(SelEllipseItem->rect().y() + SelEllipseItem->rect().height());
			if (sqrt((SelPoint.x() - EndPoint.x())*(SelPoint.x() - EndPoint.x()) + (SelPoint.y() - EndPoint.y())*(SelPoint.y() - EndPoint.y())) < 3)
			{  //ѡ�����½ǵ�
				bSelEndPoint = true;
			}
			else if (sqrt((SelPoint.x() - rightPoint.x())*(SelPoint.x() - rightPoint.x()) + (SelPoint.y() - rightPoint.y())*(SelPoint.y() - rightPoint.y())) < 3)
			{  //ѡ�����Ͻǵ�
				bSelRightPoint = true;
			}
			else if (sqrt((SelPoint.x() - leftPoint.x())*(SelPoint.x() - leftPoint.x()) + (SelPoint.y() - leftPoint.y())*(SelPoint.y() - leftPoint.y())) < 3)
			{   //ѡ�����½ǵ�
				bSelLeftPoint = true;
			}
			else if (sqrt((SelPoint.x() - StartPoint.x())*(SelPoint.x() - StartPoint.x()) + (SelPoint.y() - StartPoint.y())*(SelPoint.y() - StartPoint.y())) < 3)
			{   //ѡ�����Ͻǵ�
				bSelStartPoint = true;
			}
			else if (abs(SelPoint.x() - EndPoint.x()) < 2)
			{   //ѡ���ұ�
				bSelRightLine = true;
			}
			else if (abs(SelPoint.y() - EndPoint.y()) < 2)
			{   //ѡ���±�
				bSelBottomLine = true;
			}
			else if (abs(SelPoint.x() - StartPoint.x()) < 2)
			{   //ѡ�����
				bSelLeftLine = true;
			}
			else if (abs(SelPoint.y() - StartPoint.y()) < 2)
			{   //ѡ���ϱ�
				bSelTopLine = true;
			}
			else
			{  //ѡ��ͼԪ
				bSelItem = true;
			}
		}
		break;
		case QGraphicsPolygonItem::Type:
		{
			if ((PolygonPoints.count() == 2) ||
				((PolygonPoints.count() > 1) && (PolygonPoints.at(0) != PolygonPoints.at(PolygonPoints.count() - 1))))
			{
				CurrentSelItem = nullptr;
				return;
			}
			QGraphicsPolygonItem*SelPolygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(CurrentSelItem);
			QPolygonF polygon = SelPolygonItem->polygon();
			QPointF tempPoint;
			float fPointdis = 0;
			for (int i = 0; i < polygon.count(); i++)
			{//�ж��Ƿ�ѡ�ж���
				tempPoint = polygon.at(i);
				fPointdis = PointsDistance(tempPoint, SelPoint);
				if (fPointdis < 5)
				{
					bSelPoint = true;
					PolygonSelPos = i;
					break;
				}
			}
			if (!bSelPoint)
			{
				for (int i = 0; i < polygon.count(); i++)
				{//�ж��Ƿ�ѡ�ж���
					tempPoint = polygon.at(i);
					fPointdis = PointsDistance(tempPoint, SelPoint);
					if (i < (polygon.count() - 1))
					{
						if (PointIsOnline(SelPoint, tempPoint, polygon.at(i + 1)))
						{
							polygon.insert(i + 1, SelPoint);
							PolygonSelPos = i + 1;
							bSelPoint = true;
							break;
						}
					}
					else
					{
						if (PointIsOnline(SelPoint, tempPoint, polygon.at(0)))
						{
							polygon.push_back(SelPoint);
							PolygonSelPos = polygon.count() - 1;
							bSelPoint = true;
							break;
						}
					}
				}
			}
			//�ж��Ƿ�ѡ�б�
			if (!bSelPoint)
			{
				bSelItem = true;
			}
			bool bc;
			bc = polygon.isClosed();

			SelPolygonItem->setPolygon(polygon);
			CurrentSelItem = SelPolygonItem;
		}
		break;
		case QImageRuler::Type:
		{
			QImageRuler*SelRulerItem = qgraphicsitem_cast<QImageRuler*>(CurrentSelItem);
			StartPoint = SelRulerItem->StartPoint;
			EndPoint = SelRulerItem->EndPoint;
			if (PointsDistance(SelPoint, SelRulerItem->StartPoint) < 5)
			{
				bSelStartPoint = true;
				StartPoint = SelRulerItem->EndPoint;
			}
			else if (PointsDistance(SelPoint, SelRulerItem->EndPoint) < 5)
			{
				bSelEndPoint = true;
			}
			else
			{
				bSelItem = true;
			}
		}
		break;
		default:
			break;
		}
	}
}
/***********************************************************************
�������ƣ�CancelItemSelState

�������壺������ͼԪ��״̬����Ϊδѡ��

��ڲ�����

��������ֵ��
*************************************************************************/
void ImageScene::CancelItemSelState()
{
	for (int i = 0; i < selectedItems().count(); i++)
	{
		selectedItems().at(i)->setSelected(false);
	}
	bSelStartPoint = false;
	bSelEndPoint = false;
	bSelItem = false;
	bSelRightPoint = false;
	bSelLeftPoint = false;
	bSelLeftLine = false;
	bSelTopLine = false;
	bSelRightLine = false;
	bSelBottomLine = false;
	bSelPoint = false;
}
/************************************************************************
�������ƣ�SetCursorType

�������壺���������״

��ڲ�����
		 QPointF SelPoint    ���λ��

��������ֵ��
*************************************************************************/
void ImageScene::SetCursorType(QPointF SelPoint)
{
	QPointF rightPoint, leftPoint, startPoint1, endPoint1;
	if (CurrentSelItem != nullptr && CurrentSelItem->isSelected())
	{
		switch (CurrentSelItem->type())
		{
		case QGraphicsRectItem::Type:
		{
			QGraphicsRectItem*item = qgraphicsitem_cast<QGraphicsRectItem*>(CurrentSelItem);
			startPoint1.setX(item->rect().x());
			startPoint1.setY(item->rect().y());
			endPoint1.setX(item->rect().x() + item->rect().width());
			endPoint1.setY(item->rect().y() + item->rect().height());
			rightPoint.setX(item->rect().x() + item->rect().width());
			rightPoint.setY(item->rect().y());
			leftPoint.setX(item->rect().x());
			leftPoint.setY(item->rect().y() + item->rect().height());
			if (sqrt((SelPoint.x() - endPoint1.x())*(SelPoint.x() - endPoint1.x()) + (SelPoint.y() - endPoint1.y())*(SelPoint.y() - endPoint1.y())) < 3)
			{  //ѡ�����½ǵ�
				CurrentSelItem->setCursor(Qt::SizeFDiagCursor);
			}
			else if (sqrt((SelPoint.x() - rightPoint.x())*(SelPoint.x() - rightPoint.x()) + (SelPoint.y() - rightPoint.y())*(SelPoint.y() - rightPoint.y())) < 3)
			{  //ѡ�����Ͻǵ�
				CurrentSelItem->setCursor(Qt::SizeBDiagCursor);
			}
			else if (sqrt((SelPoint.x() - leftPoint.x())*(SelPoint.x() - leftPoint.x()) + (SelPoint.y() - leftPoint.y())*(SelPoint.y() - leftPoint.y())) < 3)
			{   //ѡ�����½ǵ�
				CurrentSelItem->setCursor(Qt::SizeBDiagCursor);
			}
			else if (sqrt((SelPoint.x() - startPoint1.x())*(SelPoint.x() - startPoint1.x()) + (SelPoint.y() - startPoint1.y())*(SelPoint.y() - startPoint1.y())) < 3)
			{   //ѡ�����Ͻǵ�
				CurrentSelItem->setCursor(Qt::SizeFDiagCursor);
			}
			else if (abs(SelPoint.x() - endPoint1.x()) < 2)
			{   //ѡ���ұ�
				CurrentSelItem->setCursor(Qt::SizeHorCursor);
			}
			else if (abs(SelPoint.y() - endPoint1.y()) < 2)
			{   //ѡ���±�
				CurrentSelItem->setCursor(Qt::SizeVerCursor);
			}
			else if (abs(SelPoint.x() - startPoint1.x()) < 2)
			{   //ѡ�����
				CurrentSelItem->setCursor(Qt::SizeHorCursor);
			}
			else if (abs(SelPoint.y() - startPoint1.y()) < 2)
			{   //ѡ���ϱ�
				CurrentSelItem->setCursor(Qt::SizeVerCursor);
			}
			else
			{  //ѡ��ͼԪ
				CurrentSelItem->setCursor(Qt::SizeAllCursor);
			}
		}
		break;
		case QGraphicsEllipseItem::Type:
		{
			QGraphicsEllipseItem*item = qgraphicsitem_cast<QGraphicsEllipseItem*>(CurrentSelItem);
			startPoint1.setX(item->rect().x());
			startPoint1.setY(item->rect().y());
			endPoint1.setX(item->rect().x() + item->rect().width());
			endPoint1.setY(item->rect().y() + item->rect().height());
			rightPoint.setX(item->rect().x() + item->rect().width());
			rightPoint.setY(item->rect().y());
			leftPoint.setX(item->rect().x());
			leftPoint.setY(item->rect().y() + item->rect().height());
			if (sqrt((SelPoint.x() - endPoint1.x())*(SelPoint.x() - endPoint1.x()) + (SelPoint.y() - endPoint1.y())*(SelPoint.y() - endPoint1.y())) < 3)
			{  //ѡ�����½ǵ�
				CurrentSelItem->setCursor(Qt::SizeFDiagCursor);
			}
			else if (sqrt((SelPoint.x() - rightPoint.x())*(SelPoint.x() - rightPoint.x()) + (SelPoint.y() - rightPoint.y())*(SelPoint.y() - rightPoint.y())) < 3)
			{  //ѡ�����Ͻǵ�
				CurrentSelItem->setCursor(Qt::SizeBDiagCursor);
			}
			else if (sqrt((SelPoint.x() - leftPoint.x())*(SelPoint.x() - leftPoint.x()) + (SelPoint.y() - leftPoint.y())*(SelPoint.y() - leftPoint.y())) < 3)
			{   //ѡ�����½ǵ�
				CurrentSelItem->setCursor(Qt::SizeBDiagCursor);
			}
			else if (sqrt((SelPoint.x() - startPoint1.x())*(SelPoint.x() - startPoint1.x()) + (SelPoint.y() - startPoint1.y())*(SelPoint.y() - startPoint1.y())) < 3)
			{   //ѡ�����Ͻǵ�
				CurrentSelItem->setCursor(Qt::SizeFDiagCursor);
			}
			else if (abs(SelPoint.x() - endPoint1.x()) < 2)
			{   //ѡ���ұ�
				CurrentSelItem->setCursor(Qt::SizeHorCursor);
			}
			else if (abs(SelPoint.y() - endPoint1.y()) < 2)
			{   //ѡ���±�
				CurrentSelItem->setCursor(Qt::SizeVerCursor);
			}
			else if (abs(SelPoint.x() - startPoint1.x()) < 2)
			{   //ѡ�����
				CurrentSelItem->setCursor(Qt::SizeHorCursor);
			}
			else if (abs(SelPoint.y() - startPoint1.y()) < 2)
			{   //ѡ���ϱ�
				CurrentSelItem->setCursor(Qt::SizeVerCursor);
			}
			else
			{  //ѡ��ͼԪ
				CurrentSelItem->setCursor(Qt::SizeAllCursor);
			}
		}
		break;
		case QGraphicsPolygonItem::Type:
		{
			QGraphicsPolygonItem*SelPolygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(CurrentSelItem);
			QPolygonF polygon = SelPolygonItem->polygon();
			QPointF tempPoint;
			float fPointdis = 0;
			for (int i = 0; i < polygon.count(); i++)
			{//�ж��Ƿ�ѡ�ж���
				tempPoint = polygon.at(i);
				fPointdis = PointsDistance(tempPoint, SelPoint);
				if (fPointdis < 5)
				{
					CurrentSelItem->setCursor(Qt::SplitVCursor);
					return;
				}
			}
			for (int i = 0; i < polygon.count(); i++)
			{
				tempPoint = polygon.at(i);
				if (i < (polygon.count() - 1))
				{
					if (PointIsOnline(SelPoint, tempPoint, polygon.at(i + 1)))
					{
						CurrentSelItem->setCursor(Qt::SizeFDiagCursor);
						return;
					}
				}
				else
				{
					if (PointIsOnline(SelPoint, tempPoint, polygon.at(0)))
					{
						CurrentSelItem->setCursor(Qt::SizeFDiagCursor);
						return;
					}
				}
			}
			//�ж��Ƿ�ѡ�б�
			CurrentSelItem->setCursor(Qt::SizeAllCursor);
		}
		break;
		case QImageRuler::Type:
		{
			QImageRuler*SelRulerItem = qgraphicsitem_cast<QImageRuler*>(CurrentSelItem);
			if (PointsDistance(SelPoint, SelRulerItem->StartPoint) < 5)
			{
				CurrentSelItem->setCursor(Qt::SizeFDiagCursor);
			}
			else if (PointsDistance(SelPoint, SelRulerItem->EndPoint) < 5)
			{
				CurrentSelItem->setCursor(Qt::SizeFDiagCursor);
			}
			else
			{
				CurrentSelItem->setCursor(Qt::SizeAllCursor);
			}
		}
		break;
		default:
			break;
		}
	}
}
/******************************************************************************
�������ƣ�PointIsOnline

�������壺�жϵ��Ƿ����߶���

��ڲ�����
		 QPointF SelPoint        ���жϵĵ�
		 QPointF SegmentStart    �߶����
		 QPointF SegmentEnd      �߶��յ�

��������ֵ��
		  true   �����߶���
		  false  �㲻���߶���
*******************************************************************************/
bool ImageScene::PointIsOnline(QPointF SelPoint, QPointF SegmentStart, QPointF SegmentEnd)
{
	float dx, dy;
	float tempx, tempy;
	float distance;
	dx = SegmentEnd.x() - SegmentStart.x();
	dy = SegmentEnd.y() - SegmentStart.y();
	distance = PointToLineDis(SelPoint, SegmentStart, SegmentEnd);
	if (distance < 5)
	{
		return true;
	}
	return false;
}
/**********************************************************************
�������ƣ�PointsDistance

�������壺����֮��ľ���

��ڲ�����
		 QPointF SegmentStart     ��һ����
		 QPointF SegmentEnd       �ڶ�����

��������ֵ��
		 float     ��������֮��ľ���
**********************************************************************/
float ImageScene::PointsDistance(QPointF SegmentStart, QPointF SegmentEnd)
{
	float linedis = 0;
	linedis = sqrt((SegmentEnd.x() - SegmentStart.x())*(SegmentEnd.x() - SegmentStart.x()) + (SegmentEnd.y() - SegmentStart.y())*(SegmentEnd.y() - SegmentStart.y()));
	return linedis;
}
/**********************************************************************
�������ƣ�PointToLineDis

�������壺�㵽�ߵľ���

��ڲ�����
		 QPointF SelPoint     �������
		 QPointF SegmentStart �ߵĵ�һ����
		 QPointF SegmentEnd   �ߵĵڶ�����

��������ֵ��
		 float   �㵽�ߵľ���
*********************************************************************/
float ImageScene::PointToLineDis(QPointF SelPoint, QPointF SegmentStart, QPointF SegmentEnd)
{
	const double MINDIST = exp(-6);
	float space = 0;
	float a, b, c;
	a = PointsDistance(SegmentStart, SegmentEnd);
	b = PointsDistance(SelPoint, SegmentStart);
	c = PointsDistance(SelPoint, SegmentEnd);
	if (b <= MINDIST || c <= MINDIST)
	{
		space = 0;
		return space;
	}
	if (a < MINDIST)
	{
		space = b;
		return space;
	}
	if (c * c >= a * a + b * b)
	{
		space = b;
		return space;
	}
	if (b * b >= a * a + c * c)
	{
		space = c;
		return space;
	}
	float p = (a + b + c) / 2;// ���ܳ�
	float s = sqrt(p * (p - a) * (p - b) * (p - c));// ���׹�ʽ�����
	space = 2 * s / a;// ���ص㵽�ߵľ��루���������������ʽ��ߣ�
	return space;
}
/****************************************************************************/
QPolygonF ImageScene::DeleteSameData(QPolygonF polygon)
{
	QPointF ppoint1, ppoint2;
	QPolygonF temPolygon;
	if (polygon.count() > 0)
	{
		temPolygon.push_back(polygon.at(0));
	}
	for (int i = 0; i < polygon.count() - 1; i++)
	{
		ppoint1 = temPolygon.at(temPolygon.count() - 1);
		ppoint2 = polygon.at(i + 1);
		if (ppoint1 != ppoint2)
		{
			temPolygon.push_back(ppoint2);
		}
	}
	return temPolygon;
}

/*******************************  End :ImageScene   ************************************/


/*******************************  Start :QImageRuler  **********************************/
/********************************************************************
�������ƣ�QImageRuler

�������壺���캯��

��ڲ�����QObject *parent

��������ֵ��
***********************************************************************/
QImageRuler::QImageRuler(QObject *parent)
	: QObject(parent)
{
	//setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
}
/*******************************************************************
�������ƣ�QImageRuler

�������壺���캯��

��ڲ�����QPointF StartP   ���������
		  QPointF EndP     �������յ�

��������ֵ��
*******************************************************************/
QImageRuler::QImageRuler(QPointF StartP, QPointF EndP, QGraphicsItem *parent)
{
	setAcceptHoverEvents(true);
	setRulerItem(StartP, EndP);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
}
/*****************************************************************
�������ƣ�~QImageRuler

�������壺��������

��ڲ�����

��������ֵ��
*******************************************************************/
QImageRuler::~QImageRuler()
{

}
/*****************************************************************
�������ƣ�boundingRect

�������壺����߽����

��ڲ�����

��������ֵ��QRectF  �߽����
****************************************************************/
QRectF QImageRuler::boundingRect() const
{

	qreal redist_x, redist_y;
	if (EndPoint1.x() < 0)
	{
		redist_x = EndPoint1.x() - 5;
		//m_width = m_width + 5;
	}
	else
	{
		redist_x = -5;
	}
	if (EndPoint1.y() < 0)
	{
		redist_y = EndPoint1.y() - 5;
	}
	else
	{
		redist_y = -5;
	}
	return QRectF(redist_x, redist_y, m_width + 10, m_height + 10);
	//return QRectF(-EndPoint1.x(), -EndPoint1.y(), m_width, m_height);
}
/*********************************************************************************
�������ƣ�paint

�������壺������״

��ڲ�����QPainter *painter
		  QStyleOptionGraphicsItem *option
		  QWidget *widget

��������ֵ��
*********************************************************************************/
void QImageRuler::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

	//pen.setColor(QColor(0, 255, 0));
	//pen.setWidth(2);
	painter->setRenderHint(QPainter::Antialiasing, true);
	if (option->state & QStyle::State_Selected)
	{
		QPen pen;
		pen.setColor(QColor(0, 255, 0));
		pen.setWidth(2);
		painter->setPen(pen);
		//painter->drawLine(StartPoint1, EndPoint1);
		painter->drawEllipse(StartPoint1, 3, 3);
		painter->drawEllipse(EndPoint1, 3, 3);
		painter->drawRect(boundingRect());
	}
	else
	{
		painter->setPen(Drawpen);
	}

	QVector <QPointF> tempf;
	float len = 10;
	float angleVal;
	painter->drawLine(StartPoint1, EndPoint1);
	QPointF sp, ep, mp, rectpoint;
	QFont font(m_string);
	font.setPointSize(14);

	font.setFamily("SimHei");
	QFontMetrics metr(font);
	int r_width = metr.width(m_string);
	int r_height = metr.height();
	mp.setX((StartPoint1.x() + EndPoint1.x()) / 2);
	mp.setY((StartPoint1.y() + EndPoint1.y()) / 2);
	rectpoint = GetTextPoint(mp, r_width, r_height, angleVal);


	sp.setX(StartPoint1.x() - 5 * sin(angleVal));
	sp.setY(StartPoint1.y() - 5 * cos(angleVal));
	ep.setX(StartPoint1.x() + 5 * sin(angleVal));
	ep.setY(StartPoint1.y() + 5 * cos(angleVal));
	painter->drawLine(sp, ep);
	sp.setX(EndPoint1.x() - 5 * sin(angleVal));
	sp.setY(EndPoint1.y() - 5 * cos(angleVal));
	ep.setX(EndPoint1.x() + 5 * sin(angleVal));
	ep.setY(EndPoint1.y() + 5 * cos(angleVal));
	painter->drawLine(sp, ep);

	QRect rtext(rectpoint.x(), rectpoint.y(), r_width, r_height);

	painter->translate(qreal(rectpoint.x()), qreal(rectpoint.y()));


	angleVal = angleVal * 180 / 3.14159;

	painter->rotate(-angleVal);

	painter->drawText(15, 0, m_string);

}
/******************************************************************************
�������ƣ�GetTextPoint

�������壺�����д�ı�����ʼ��

��ڲ�����

��������ֵ��
*********************************************************************************/
QPointF QImageRuler::GetTextPoint(QPointF point, int width, int height, float &angle)
{
	float r_len = width / 2;
	height = height / 2;
	QPointF temp, temps;
	float x1, y1, x2, y2;
	float offsetx, tempangle;
	float dx = EndPoint1.x();   //x����ĳ���
	float dy = EndPoint1.y();   //y����ĳ���
	float dist = sqrt(dx*dx + dy * dy);
	x1 = StartPoint1.x();
	y1 = m_height - StartPoint1.y();
	x2 = EndPoint1.x();
	y2 = m_height - EndPoint1.y();
	offsetx = (dist - width) / 2;
	if (x1 == x2)
	{
		angle = -3.1415 / 2;
		if (dy > 0)
		{
			temp.setX(height);
			temp.setY(offsetx);
			//temp.setY(EndPoint1.y() -offsetx);
		}
		else
		{
			temp.setX(height);
			temp.setY(EndPoint1.y() + offsetx);
			//temp.setY(-offsetx);
		}

	}
	else
	{
		k = (y2 - y1) / (x2 - x1);
		angle = atan(k);
		if (abs(dx) > abs(dy))                    //�ߵ����С��45�Ȼ��ߴ���135��
		{
			tempangle = atan(abs(k));
			float offset_x, offset_y;
			offset_x = offsetx * abs(cos(tempangle));
			offset_y = offsetx * sin(tempangle);
			if (k > 0)
			{

				if (dx > 0)
				{
					temps.setX(offset_x);
					temps.setY(-offset_y);
					temp.setX(temps.x() - height * sin(tempangle));
					temp.setY(temps.y() - height * cos(tempangle));
				}
				else
				{
					temps.setX(EndPoint1.x() + offset_x);
					temps.setY(EndPoint1.y() - offset_y);
					temp.setX(temps.x() - height * sin(tempangle));
					temp.setY(temps.y() - height * cos(tempangle));
				}
			}
			else
			{
				if (dx > 0)
				{
					temps.setX(offset_x);
					temps.setY(offset_y);
					temp.setX(temps.x() + height * sin(tempangle));
					temp.setY(temps.y() - height * abs(cos(tempangle)));
				}
				else
				{
					temps.setX(EndPoint1.x() + offset_x);
					temps.setY(EndPoint1.y() + offset_y);
					temp.setX(temps.x() + height * sin(tempangle));
					temp.setY(temps.y() - height * abs(cos(tempangle)));
				}

			}

		}
		else
		{
			tempangle = atan(abs(k));
			float offset_x, offset_y;
			offset_x = offsetx * abs(cos(tempangle));
			offset_y = offsetx * sin(tempangle);
			if (k > 0)
			{
				if (dy < 0)
				{
					temps.setX(offset_x);
					temps.setY(-offset_y);
					temp.setX(temps.x() - height * sin(tempangle));
					temp.setY(temps.y() - height * abs(cos(tempangle)));
				}
				else
				{
					temps.setX(EndPoint1.x() + offset_x);
					temps.setY(EndPoint1.y() - offset_y);
					temp.setX(temps.x() - height * sin(tempangle));
					temp.setY(temps.y() - height * abs(cos(tempangle)));
				}
			}
			else
			{
				if (dy > 0)
				{
					temps.setX(offset_x);
					temps.setY(offset_y);
					temp.setX(temps.x() + height * sin(tempangle));
					temp.setY(temps.y() - height * abs(cos(tempangle)));
				}
				else
				{
					temps.setX(EndPoint1.x() + offset_x);
					temps.setY(EndPoint1.y() + offset_y);
					temp.setX(temps.x() + height * sin(tempangle));
					temp.setY(temps.y() - height * abs(cos(tempangle)));
				}

			}

		}
	}
	return temp;
}
/****************************************************************************
�������ƣ�setRulerItem

�������壺���óߴ������λ��

��ڲ�����QPointF StartP    �������
		  QPointF EndP      �����յ�

��������ֵ��
***************************************************************************/
void QImageRuler::setRulerItem(QPointF StartP, QPointF EndP)
{
	StartPoint = StartP;
	EndPoint = EndP;
	float dx = EndP.x() - StartP.x();
	float dy = EndP.y() - StartP.y();
	float dist = sqrt(dx*dx + dy * dy);
	m_string = QString::number(dist, 'f', 3);
	m_string = m_string + "mm";
	BasePoint.setX(fmin(StartP.x(), EndP.x()));
	BasePoint.setY(fmin(StartP.y(), EndP.y()));

	StartPoint1 = QPointF(0, 0);
	EndPoint1 = EndP - StartP;
	m_width = abs(EndP.x() - StartP.x());// +50;
	m_height = abs(EndP.y() - StartP.y());// +50;
	this->setPos(StartP);
}
/********************************************************************************
�������ƣ�setPen

�������壺����ͼԪ����

��ڲ�����QPen pen    ���û�������

��������ֵ��
********************************************************************************/
void QImageRuler::setPen(QPen pen)
{
	Drawpen = pen;
}
/*****************************************************************************
�������ƣ�pen

�������壺���ͼԪ��Pen����

��ڲ�����

��������ֵ��QPen   ����ͼԪ��pen
*****************************************************************************/
QPen QImageRuler::pen()
{
	return Drawpen;
}
/*******************************  End :QImageRuler  **********************************/

