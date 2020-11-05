#include "stdafx.h"		
#include "imagewidget.h"
#include "imagewidgetmanager.h"

//NOTEPAD//
//����widget��gridlayout��sizeconstraint���ó�noconstraint��Ȼ������������Сsize��
//����Զ���widget���������С���š�
//ʹ������sizeconstraint����defaultconstraint��������ƣ�������ʾfoldtreeʱ�ᵼ��ʵ����ʾ�������flodtree
//�Ŀ�ȼ���imagelabel�Ŀ�ȣ�ԭ���������������С�ߴ磬��������������С�ߴ�ʹ��ʾ������������ֲ�����������
//���š�
//NOTEPAD//

//NOTEPAD//
//ʹ���ĸ�spacer���������ܣ���ʹ��ͬ�������Ų������������м�Ĳ���ʼ�վ�����ʾ
//NOTEPAD//

QVector<QRgb> ImageWidget::d_colorTable = initializeColorTable();
const int ImageWidget::d_initialZoomRatioFactor = 10;

QVector<QRgb> ImageWidget::initializeColorTable()
{
	QVector<QRgb> colorTable;

	for (int i = 0; i != 256; ++i)
		colorTable.push_back(qRgb(i, i, i));

	return colorTable;
}
ImageWidget::ImageWidget(ImageWidgetManager* _imageWidgetManager, int _screenWidth, int _screenHeight, QWidget *parent)
	: d_manager(_imageWidgetManager), QWidget(parent), d_screenWidth(_screenWidth), d_screenHeight(_screenHeight)
{
	ui.setupUi(this);
	QString str;
	loadImage(str);
	ui.imageLabel->setAlignment(Qt::AlignCenter);

	if (float(d_screenWidth) / d_screenHeight > float(d_imageWidth) / d_imageHeight)
	{
		resize(d_screenWidth * 0.6, d_screenHeight * 0.6 * d_imageHeight / d_imageWidth);
	}
	else
	{
		resize(d_screenHeight * 0.6 * d_imageWidth / d_imageHeight, d_screenHeight * 0.6);
	}
}

ImageWidget::ImageWidget(ImageWidgetManager* _imageWidgetManager, int _screenWidth, int _screenHeight,
	unsigned char* _buffer, int _width, int _height, QWidget *parent)
	: d_manager(_imageWidgetManager), QWidget(parent), d_screenWidth(_screenWidth), d_screenHeight(_screenHeight)
	, d_pixmap(new QPixmap), d_imageWidth(_width), d_imageHeight(_height)
{
	ui.setupUi(this);
	d_imageBuffer = (unsigned char*)malloc(_width * _height);
	initialWindowSize();
	loadImage(_buffer, _width, _height);
	setMouseTracking(true);
	ui.imageLabel->setMouseTracking(true);
}

ImageWidget::ImageWidget(ImageWidgetManager* _imageWidgetManager, int _screenWidth, int _screenHeight, 
	QString& _fileName, QWidget *parent)
	: d_manager(_imageWidgetManager), QWidget(parent), d_screenWidth(_screenWidth), d_screenHeight(_screenHeight)
	, d_pixmap(new QPixmap)
{
	ui.setupUi(this);
	initialWindowSize();
	loadImage(_fileName);
	setMouseTracking(true);
	ui.imageLabel->setMouseTracking(true);
}

ImageWidget::~ImageWidget()
{
	free(d_imageBuffer);
}

void ImageWidget::initialWindowSize()
{
	d_imageTopLeftXOnImage = 0;
	d_imageTopLeftYOnImage = 0;
	float wideRatio = float(d_screenWidth) / d_imageWidth;
	float heightRatio = float(d_screenHeight) / d_imageHeight;

	if (wideRatio >= heightRatio)
		if (heightRatio >= 2)
			d_zoomRatio = 1; //�Ը߶�Ϊ׼
		else
			d_zoomRatio = float(d_screenHeight) / d_imageHeight * 0.6;
	else
		if (wideRatio >= 2)
			d_zoomRatio = 1; //�Կ��Ϊ׼
		else
			d_zoomRatio = float(d_screenWidth) / d_imageWidth * 0.6;

	d_zoomRecommendRatio = d_zoomRatio;
}

bool ImageWidget::loadImage(QString& _fileName)
{
	QImage image("D:/code/CTScan/CTScan_QT/x64/Debug/gain.tif");
	d_pixmap->convertFromImage(image);
	return true;
}

bool ImageWidget::loadImage(unsigned char* _buffer, int _width, int _height)
{
	//for (int i = 0; i != _height; ++i)
	//	for (int j = 0; j != _width; ++j)
	//		d_imageBuffer[i * _width + j] = ((unsigned short*)_buffer)[i * _width + j] / 256;

	//DEBUG���������Ź���
	for (int i = 0; i != _height; ++i)
		for (int j = 0; j != _width; ++j)
			d_imageBuffer[i * _width + j] = 0;
	
	for (int j = 1000; j != 1048; ++j)
		for(int i = 1019; i != 1029; ++i)
			d_imageBuffer[i * 2048 + j] = 255;

	for (int j = 1000; j != 1048; ++j)
		for (int i = 1019; i != 1029; ++i)
			d_imageBuffer[j * 2048 + i] = 255;
	//DEBUG���������Ź���

	d_image.reset(new QImage(d_imageBuffer, _width, _height, _width * sizeof(unsigned char), QImage::Format_Indexed8));
	d_imageWidth = d_image->width();
	d_imageHeight = d_image->height();
	d_image->setColorTable(d_colorTable);
	d_pixmap->convertFromImage(*d_image);
	resizeWidget();
	return true;
}

void ImageWidget::resizeWidget()
{
	d_zoomRatio = d_zoomRecommendRatio * d_zoomRatioFactor / d_initialZoomRatioFactor;
	int lineWidth = ui.imageLabel->lineWidth() * 2;
	d_imageScreenWidth = d_pixmap->width() * d_zoomRatio;
	d_imageScreenHeight = d_pixmap->height() * d_zoomRatio;
	ui.imageLabel->setFixedSize(d_imageScreenWidth + lineWidth, d_imageScreenHeight + lineWidth);
	d_imageTopLeftXOnImage = 0;
	d_imageTopLeftYOnImage = 0;
	zoomImage();
	resize(d_imageScreenWidth + lineWidth + ui.folderTree->width() + ui.foldButton->width() +
		ui.horizontalSpacer->minimumSize().width() * 2,
		d_imageScreenHeight + lineWidth + ui.messageLabel->height() +
		ui.verticalSpacer->minimumSize().height() * 2);
}

void ImageWidget::zoomOut()
{
	if (!ui.imageLabel->underMouse())
		return;

	if (d_zoomRatioFactor >= d_initialZoomRatioFactor)
	{
		int x = d_mousePos.x();
		int y = d_mousePos.y();
		caculateMousePosOnImage(x, y);
		d_zoomRatioFactor += 1;
		d_zoomRatio = d_zoomRecommendRatio * d_zoomRatioFactor / d_initialZoomRatioFactor;
		d_mousePosOnImage.setX(d_mousePosOnImage.x() * d_zoomRatio);
		d_mousePosOnImage.setY(d_mousePosOnImage.y() * d_zoomRatio);
		d_imageTopLeftXOnImage = d_mousePosOnImage.x() - (d_mousePosOnImageLabel.x() - (ui.imageLabel->width() - d_imageScreenWidth) / 2);
		d_imageTopLeftYOnImage = d_mousePosOnImage.y() - (d_mousePosOnImageLabel.y() - (ui.imageLabel->height() - d_imageScreenHeight) / 2);
	}
	else
	{
		int x = d_mousePos.x();
		int y = d_mousePos.y();
		caculateMousePosOnImage(x, y);
		d_zoomRatioFactor += 1;
		resizeWidget();
		d_mousePosOnImage.setX(d_mousePosOnImage.x() * d_zoomRatio);
		d_mousePosOnImage.setY(d_mousePosOnImage.y() * d_zoomRatio);
		float ratio = float(1) / (d_zoomRatioFactor - 1);
		move(pos().x() - d_mousePosOnImageLabel.x() * ratio, pos().y() - d_mousePosOnImageLabel.y() * ratio);
	}

	zoomImage();
}
void ImageWidget::zoomIn()
{
	if (!ui.imageLabel->underMouse())
		return;

	if (d_zoomRatioFactor <= d_initialZoomRatioFactor + 1)
	{
		int x = d_mousePos.x();
		int y = d_mousePos.y();
		caculateMousePosOnImage(x, y);
		d_zoomRatioFactor -= 1;
		resizeWidget();
		//TODO�����¼���d_mousePos;
		d_mousePosOnImage.setX(d_mousePosOnImage.x() * d_zoomRatio);
		d_mousePosOnImage.setY(d_mousePosOnImage.y() * d_zoomRatio);

		if (d_zoomRatioFactor < d_initialZoomRatioFactor)
		{
			float ratio = float(1) / (d_zoomRatioFactor + 1);
			move(pos().x() + d_mousePosOnImageLabel.x() * ratio, pos().y() + d_mousePosOnImageLabel.y() * ratio);
		}
	}
	else
	{
		int x = d_mousePos.x();
		int y = d_mousePos.y();
		caculateMousePosOnImage(x, y);
		d_zoomRatioFactor -= 1;
		d_zoomRatio = d_zoomRecommendRatio * d_zoomRatioFactor / d_initialZoomRatioFactor;
		d_mousePosOnImage.setX(d_mousePosOnImage.x() * d_zoomRatio);
		d_mousePosOnImage.setY(d_mousePosOnImage.y() * d_zoomRatio);
		d_imageTopLeftXOnImage = d_mousePosOnImage.x() - (d_mousePosOnImageLabel.x() - (ui.imageLabel->width() - d_imageScreenWidth) / 2);
		d_imageTopLeftYOnImage = d_mousePosOnImage.y() - (d_mousePosOnImageLabel.y() - (ui.imageLabel->height() - d_imageScreenHeight) / 2);
	}

	zoomImage();
}

void ImageWidget::showEvent(QShowEvent* _event)
{
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (!ui.imageLabel->underMouse())
		return;

	d_mousePos = event->pos();
	int posX, posY;
	
	if (!caculateMousePosOnImage(posX, posY))
		return;

	QString mousePosStr;
	mousePosStr.sprintf("%d, %d", posX, posY);
	ui.messageLabel->setText(mousePosStr);
}
bool ImageWidget::caculateMousePosOnImage(int& _posX, int& _posY)
{
	d_imageLabelRect = ui.imageLabel->geometry();

	if (d_mousePos.x() >= d_imageLabelRect.right() || d_mousePos.x() <= d_imageLabelRect.left() || 
		d_mousePos.y() >= d_imageLabelRect.bottom() || d_mousePos.y() <= d_imageLabelRect.top())
		return false;

	int labelWidth = ui.imageLabel->width();
	int labelHeight = ui.imageLabel->height();
	int imageScreenTopPos = (labelHeight - d_imageScreenHeight) / 2;
	int imageScreenDownPos = imageScreenTopPos + d_imageScreenHeight;
	int imageScreenLeftPos = (labelWidth - d_imageScreenWidth) / 2;
	int imageScreenRightPos = imageScreenLeftPos + d_imageScreenWidth;

	if (d_mousePos.x() - d_imageLabelRect.left() > imageScreenLeftPos && d_mousePos.x() - d_imageLabelRect.left() < imageScreenRightPos &&
		d_mousePos.y() - d_imageLabelRect.top() > imageScreenTopPos && d_mousePos.y() - d_imageLabelRect.top() < imageScreenDownPos)
	{
		d_mousePosToImageLeft = d_mousePos.x() - d_imageLabelRect.left() - imageScreenLeftPos;
		d_mousePosToImageRight = d_mousePos.y() - d_imageLabelRect.top() - imageScreenTopPos;
		_posX = (d_mousePosToImageLeft + d_imageTopLeftXOnImage) / d_zoomRatio;
		_posY = (d_mousePosToImageRight + d_imageTopLeftYOnImage) / d_zoomRatio;
		d_mousePosOnImageLabel.setX(d_mousePos.x() - d_imageLabelRect.left());
		d_mousePosOnImageLabel.setY(d_mousePos.y() - d_imageLabelRect.top());
		d_mousePosOnImage.setX(_posX);
		d_mousePosOnImage.setY(_posY);
		return true;
	}
	else
		return false;

}

void ImageWidget::resizeEvent(QResizeEvent *event)
{
}
void ImageWidget::on_foldButton_clicked()
{
	if (ui.folderTree->isVisible())
	{
		int wid = width();
		int newWidth = width() - ui.folderTree->width();
		ui.folderTree->hide();
		ui.foldButton->setText(tr("<"));
		resize(newWidth, height());
	}
	else
	{
		QLayout::SizeConstraint i  = ui.gridLayout->sizeConstraint();
		int wid = maximumWidth();
		int hei = maximumHeight();
		int floderWidth = ui.folderTree->width();
		int newWidth = width() + ui.folderTree->width();
		ui.folderTree->show();
		ui.foldButton->setText(tr(">"));
		resize(newWidth, height());
	}
}


void ImageWidget::keyPressEvent(QKeyEvent* _event)
{
	switch (_event->key())
	{
	case Qt::Key_Minus:
		zoomIn();
		break;
	case Qt::Key_Plus:
		zoomOut();
		break;
	}
}
void ImageWidget::zoomImage()
{
	d_zoomedPixMap = d_pixmap->scaledToWidth(d_pixmap->width() * d_zoomRatio);
	QPixmap copy = d_zoomedPixMap.copy(d_imageTopLeftXOnImage, d_imageTopLeftYOnImage, d_imageScreenWidth, d_imageScreenHeight);
	ui.imageLabel->clear();
	ui.imageLabel->setPixmap(copy);
}