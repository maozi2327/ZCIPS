#include "stdafx.h"		
#include "imagedialog.h"
#include "imagedialogmanager.h"

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

QVector<QRgb> ImageDialog::d_colorTable = initializeColorTable();
const int ImageDialog::d_initialZoomRatioFactor = 10;

QVector<QRgb> ImageDialog::initializeColorTable()
{
	QVector<QRgb> colorTable;

	for (int i = 0; i != 256; ++i)
		colorTable.push_back(qRgb(i, i, i));

	return colorTable;
}

ImageDialog::ImageDialog(ImageDialogManager* _imageWidgetManager, int _screenWidth, int _screenHeight, QWidget *parent)
	: d_manager(_imageWidgetManager), QDialog(parent), d_screenWidth(_screenWidth), d_screenHeight(_screenHeight)
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

ImageDialog::ImageDialog(ImageDialogManager* _imageWidgetManager, int _screenWidth, int _screenHeight,
	unsigned char* _buffer, int _width, int _height, QWidget *parent)
	: d_manager(_imageWidgetManager), QDialog(parent), d_screenWidth(_screenWidth), d_screenHeight(_screenHeight)
	, d_pixmap(new QPixmap)//, d_imageWidth(_width), d_imageHeight(_height)
	, d_imageWidth(2048), d_imageHeight(2048)
{
	ui.setupUi(this);
	//DEBUG������ͼ�����ű�ע�ͣ����δ����_width��_heightû��ֵ
	//d_imageBuffer = (unsigned char*)malloc(_width * _height);
	d_imageBuffer = (unsigned char*)malloc(2048 * 2048);
	initialWindowSize();
	//loadImage(_buffer, _width, _height);
	loadImage(_buffer, 2048, 2048);
	setMouseTracking(true);
	ui.imageLabel->setMouseTracking(true);
}

ImageDialog::ImageDialog(ImageDialogManager* _imageWidgetManager, int _screenWidth, int _screenHeight, 
	QString& _fileName, QWidget *parent)
	: d_manager(_imageWidgetManager), QDialog(parent), d_screenWidth(_screenWidth), d_screenHeight(_screenHeight)
	, d_pixmap(new QPixmap)
{
	ui.setupUi(this);
	initialWindowSize();
	loadImage(_fileName);
	setMouseTracking(true);
	ui.imageLabel->setMouseTracking(true);
}

ImageDialog::~ImageDialog()
{
	free(d_imageBuffer);
}

void ImageDialog::initialWindowSize()
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

bool ImageDialog::loadImage(QString& _fileName)
{
	QImage image("D:/code/CTScan/CTScan_QT/x64/Debug/gain.tif");
	d_pixmap->convertFromImage(image);
	return true;
}

bool ImageDialog::loadImage(unsigned char* _buffer, int _width, int _height)
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
	zoomImage();
	return true;
}

void ImageDialog::resizeWidget()
{
	int lineWidth = ui.imageLabel->lineWidth() * 2;
	d_imageScreenWidth = d_pixmap->width() * d_zoomRatio;
	d_imageScreenHeight = d_pixmap->height() * d_zoomRatio;
	ui.imageLabel->setFixedSize(d_imageScreenWidth + lineWidth, d_imageScreenHeight + lineWidth);
	resize(d_imageScreenWidth + lineWidth +
		ui.horizontalSpacer->minimumSize().width() * 2,
		d_imageScreenHeight + lineWidth + ui.messageLabel->height() +
		ui.verticalSpacer->minimumSize().height() * 2);
}

void ImageDialog::resizeAndMoveWidget(int _zoomFactorVariation)
{
	auto widgetLeftTop = pos();
	resizeWidget();
	float ratio1 = float(d_zoomRatioFactor) / (d_zoomRatioFactor - _zoomFactorVariation);
	d_mousePosToImageLeft = d_mousePosToImageLeft * ratio1;
	d_mousePosToImageTop = d_mousePosToImageTop * ratio1;
	float ratio = float(_zoomFactorVariation) / (d_zoomRatioFactor);
	move(widgetLeftTop.x() - d_mousePosToImageLeft * ratio,
		widgetLeftTop.y() - d_mousePosToImageTop * ratio);
	d_imageLabelRect = ui.imageLabel->geometry();
	d_mousePos.setX(d_mousePosToImageLeft + d_imageLabelRect.left() + 1 + 1);
	d_mousePos.setY(d_mousePosToImageTop + d_imageLabelRect.top() + 1 + 1);
}

void ImageDialog::zoomOut()
{
	if (!ui.imageLabel->underMouse())
		return;

	int x = d_mousePos.x();
	int y = d_mousePos.y();
	caculateMousePosOnImage(x, y);
	d_zoomRatioFactor += 1;
	d_zoomRatio = d_zoomRecommendRatio * d_zoomRatioFactor / d_initialZoomRatioFactor;
	d_mousePosOnImage.setX(d_mousePosOnImage.x() * d_zoomRatio);
	d_mousePosOnImage.setY(d_mousePosOnImage.y() * d_zoomRatio);

	if (d_zoomRatioFactor <= d_initialZoomRatioFactor)
	{
		d_imageTopLeftXOnImage = 0;
		d_imageTopLeftYOnImage = 0;
		resizeAndMoveWidget(1);
	}
	else
	{
		d_imageTopLeftXOnImage = d_mousePosOnImage.x() - d_mousePosToImageLeft;
		d_imageTopLeftYOnImage = d_mousePosOnImage.y() - d_mousePosToImageTop;
	}
		
	zoomImage();
}

void ImageDialog::zoomIn()
{
	if (!ui.imageLabel->underMouse())
		return;

	int x = d_mousePos.x();
	int y = d_mousePos.y();
	caculateMousePosOnImage(x, y);
	d_zoomRatioFactor += -1;
	d_zoomRatio = d_zoomRecommendRatio * d_zoomRatioFactor / d_initialZoomRatioFactor;
	d_mousePosOnImage.setX(d_mousePosOnImage.x() * d_zoomRatio);
	d_mousePosOnImage.setY(d_mousePosOnImage.y() * d_zoomRatio);

	if (d_zoomRatioFactor <= d_initialZoomRatioFactor)
	{
		d_imageTopLeftXOnImage = 0;
		d_imageTopLeftYOnImage = 0;

		if(d_zoomRatioFactor < d_initialZoomRatioFactor)
			resizeAndMoveWidget(-1);
		else
		{
			//��������λ����ͼ����Сǰ��ԭʼͼ���ϵ��߿�����Ϊd_mousePosToImageLeft/Top
			//��������λ����ͼ����С֮��image�ϵ����꣬���ڴ�ʱ���ű���Ϊԭʼ������
			//���������ǵ㵽�߿������d_mousePosOnImage
			//��������λ�õĲ�
			auto widgetLeftTop = pos();
			float ratio = float(1) / (d_zoomRatioFactor);
			move(widgetLeftTop.x() - (d_mousePosOnImage.x() - d_mousePosToImageLeft),
				widgetLeftTop.y() - (d_mousePosOnImage.y() - d_mousePosToImageTop));
			d_imageLabelRect = ui.imageLabel->geometry();
			d_mousePos.setX(d_mousePosToImageLeft + d_imageLabelRect.left() + 1 + 1);
			d_mousePos.setY(d_mousePosToImageTop + d_imageLabelRect.top() + 1 + 1);
		}
	}
	else
	{
		d_imageTopLeftXOnImage = d_mousePosOnImage.x() - d_mousePosToImageLeft;
		d_imageTopLeftYOnImage = d_mousePosOnImage.y() - d_mousePosToImageTop;
	}

	zoomImage();
}

void ImageDialog::showEvent(QShowEvent* _event)
{
}

void ImageDialog::mouseMoveEvent(QMouseEvent *event)
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

void ImageDialog::isOnImageLabel(int& _posX, int& _posY)
{
	
}

bool ImageDialog::caculateMousePosOnImage(int& _posX, int& _posY)
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
		d_mousePosToImageLeft = d_mousePos.x() - d_imageLabelRect.left() - imageScreenLeftPos - 1;
		d_mousePosToImageTop = d_mousePos.y() - d_imageLabelRect.top() - imageScreenTopPos - 1;
		_posX = (d_mousePosToImageLeft + d_imageTopLeftXOnImage) / d_zoomRatio;
		_posY = (d_mousePosToImageTop + d_imageTopLeftYOnImage) / d_zoomRatio;
		d_mousePosOnImage.setX(_posX);
		d_mousePosOnImage.setY(_posY);
		return true;
	}
	else
		return false;

}

void ImageDialog::resizeEvent(QResizeEvent *event)
{
}

void ImageDialog::on_foldButton_clicked()
{
	//if (ui.folderTree->isVisible())
	//{
	//	int wid = width();
	//	int newWidth = width() - ui.folderTree->width();
	//	ui.folderTree->hide();
	//	ui.foldButton->setText(tr("<"));
	//	resize(newWidth, height());
	//}
	//else
	//{
	//	QLayout::SizeConstraint i  = ui.gridLayout->sizeConstraint();
	//	int wid = maximumWidth();
	//	int hei = maximumHeight();
	//	int floderWidth = ui.folderTree->width();
	//	int newWidth = width() + ui.folderTree->width();
	//	ui.folderTree->show();
	//	ui.foldButton->setText(tr(">"));
	//	resize(newWidth, height());
	//}
}


void ImageDialog::keyPressEvent(QKeyEvent* _event)
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
void ImageDialog::zoomImage()
{
	d_zoomedPixMap = d_pixmap->scaledToWidth(d_pixmap->width() * d_zoomRatio);
	QPixmap copy = d_zoomedPixMap.copy(d_imageTopLeftXOnImage, d_imageTopLeftYOnImage, d_imageScreenWidth, d_imageScreenHeight);
	ui.imageLabel->clear();
	ui.imageLabel->setPixmap(copy);
}