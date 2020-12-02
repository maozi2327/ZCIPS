#include "stdafx.h"		
#include "imagedialog.h"
#include "imagedialogmanager.h"

//NOTEPAD//
//对主widget的gridlayout的sizeconstraint设置成noconstraint，然后不设置最大和最小size，
//则可以对主widget进行任意大小缩放。
//使用其他sizeconstraint例如defaultconstraint则会有限制，例如显示foldtree时会导致实际显示面积大于flodtree
//的宽度加上imagelabel的宽度，原因是设置了最大最小尺寸，如果设置了最大最小尺寸使显示面积正常，则又不能任意拉伸
//缩放。
//NOTEPAD//

//NOTEPAD//
//使用四个spacer布局在四周，且使用同样的缩放参数，可以让中间的布局始终居中显示
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

ImageDialog::ImageDialog(int _screenWidth, int _screenHeight, int _width, int _height,
	QWidget *parent)
	: QDialog(parent), d_screenWidth(_screenWidth), d_screenHeight(_screenHeight)
	, d_pixmap(new QPixmap)//, d_imageWidth(_width), d_imageHeight(_height)
	, d_imageWidth(2048), d_imageHeight(2048)
{
	ui.setupUi(this);
	//DEBUG：测试图像缩放被注释，面板未连接_width和_height没有值
	//d_imageBuffer = (unsigned char*)malloc(_width * _height);
	d_imageBuffer = (unsigned char*)malloc(2048 * 2048);
	initialWindowSize();
	setMouseTracking(true);
	ui.imageLabel->setMouseTracking(true);
}

ImageDialog::ImageDialog(int _screenWidth, int _screenHeight,
	unsigned char* _buffer, int _width, int _height, QWidget *parent)
	: QDialog(parent), d_screenWidth(_screenWidth), d_screenHeight(_screenHeight)
	, d_pixmap(new QPixmap)//, d_imageWidth(_width), d_imageHeight(_height)
	, d_imageWidth(2048), d_imageHeight(2048)
{
	ui.setupUi(this);
	//DEBUG：测试图像缩放被注释，面板未连接_width和_height没有值
	//d_imageBuffer = (unsigned char*)malloc(_width * _height);
	d_imageBuffer = (unsigned char*)malloc(2048 * 2048);
	initialWindowSize();
	//loadImage(_buffer, _width, _height);
	loadImage(_buffer, 2048, 2048);
	setMouseTracking(true);
	ui.imageLabel->setMouseTracking(true);
}

ImageDialog::ImageDialog(int _screenWidth, int _screenHeight, 
	QString& _fileName, QWidget *parent)
	: QDialog(parent), d_screenWidth(_screenWidth), d_screenHeight(_screenHeight)
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
			d_zoomRatio = 1; //以高度为准
		else
			d_zoomRatio = float(d_screenHeight) / d_imageHeight * 0.6;
	else
		if (wideRatio >= 2)
			d_zoomRatio = 1; //以宽度为准
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
	//if (_width != d_imageWidth || _height != d_imageHeight)
	//	return false;
	//
	//for (int i = 0; i != _height; ++i)
	//	for (int j = 0; j != _width; ++j)
	//		d_imageBuffer[i * _width + j] = ((unsigned short*)_buffer)[i * _width + j] / 256;

	//DEBUG：测试缩放功能
	for (int i = 0; i != _height; ++i)
		for (int j = 0; j != _width; ++j)
			d_imageBuffer[i * _width + j] = 0;
	
	for (int j = 1000; j != 1048; ++j)
		for(int i = 1019; i != 1029; ++i)
			d_imageBuffer[i * 2048 + j] = 255;

	for (int j = 1000; j != 1048; ++j)
		for (int i = 1019; i != 1029; ++i)
			d_imageBuffer[j * 2048 + i] = 255;
	//DEBUG：测试缩放功能

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
			//计算鼠标点位置在图像缩小前在原始图像上到边框坐标为d_mousePosToImageLeft/Top
			//计算鼠标点位置在图像缩小之后image上的坐标，由于此时缩放比例为原始比例，
			//则该坐标就是点到边框的坐标d_mousePosOnImage
			//计算两个位置的差
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