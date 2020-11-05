#pragma once

#include <QWidget>
#include "ui_imagewidget.h"
#include <memory>

class ImageWidgetManager;
class ImageWidget : public QWidget
{
	Q_OBJECT

public:
	ImageWidget(ImageWidgetManager* _imageWidgetManager, int _screenWidth, int _screenHeight, QWidget *parent = Q_NULLPTR);
	ImageWidget(ImageWidgetManager* _imageWidgetManager, int _screenWidth, int _screenHeight, unsigned char* _buffer,
		int _width, int _height, QWidget *parent = Q_NULLPTR);
	ImageWidget(ImageWidgetManager* _imageWidgetManager, int _screenWidth, int _screenHeight, QString& _fileName,
		QWidget *parent = Q_NULLPTR);
	~ImageWidget();

	bool loadImage(QString& _fileName);
	bool loadImage(unsigned char* _buffer, int _width, int _height);
	void resizeWidget();
	void zoomOut();
	void zoomIn();
protected:
	virtual void showEvent(QShowEvent* _event);
	virtual void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent * _event);
	void mouseMoveEvent(QMouseEvent *event);
	void zoomImage();
	void initialWindowSize();
private:
	Ui::ImageWidget ui;
	ImageWidgetManager* d_manager;
	std::unique_ptr<QPixmap> d_pixmap;
	QPixmap d_zoomedPixMap;
	std::unique_ptr<QImage> d_image;
	unsigned char* d_imageBuffer;
	int d_imageWidth;
	int d_imageHeight;
	int d_imageScreenWidth;
	int d_imageScreenHeight;
	int d_screenWidth;
	int d_screenHeight;
	int d_initialdesktopWidth;
	int d_initialWindowHeight;
	int d_imageTopLeftXOnLabel;
	int d_imageTopLeftYOnLabel;
	int d_imageTopLeftXOnImage;
	int d_imageTopLeftYOnImage;
	int d_mousePosToImageLeft;
	int d_mousePosToImageRight;
	QPoint d_mousePos;
	QPoint d_mousePosOnImageLabel;
	QPoint d_mousePosOnImage;
	QRect d_imageLabelRect;
	static QVector<QRgb> d_colorTable;
	static QVector<QRgb> initializeColorTable();
	float d_zoomRatio = 0.7f;
	int d_zoomRatioFactor = 10;
	float d_zoomRecommendRatio;
	static const int d_initialZoomRatioFactor;
	bool caculateMousePosOnImage(int& _posX, int& _posY);
private slots:
	void on_foldButton_clicked();
};
