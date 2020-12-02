#pragma once

#include <QWidget>
#include "ui_imagedialog.h"
#include <memory>

class ImageDialogManager;
class ImageDialog : public QDialog
{
	Q_OBJECT

public:
	ImageDialog(int _screenWidth, int _screenHeight, int _width, int _height, 
		QWidget *parent = Q_NULLPTR);
	ImageDialog(int _screenWidth, int _screenHeight, unsigned char* _buffer,
		int _width, int _height, QWidget *parent = Q_NULLPTR);
	ImageDialog(int _screenWidth, int _screenHeight, QString& _fileName, QWidget *parent = Q_NULLPTR);
	~ImageDialog();

	bool loadImage(QString& _fileName);
	bool loadImage(unsigned char* _buffer, int _width, int _height);
protected:
	virtual void showEvent(QShowEvent* _event);
	virtual void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent * _event);
	void mouseMoveEvent(QMouseEvent *event);
	void isOnImageLabel(int& _posX, int& _posY);
	void zoomImage();
	void initialWindowSize();
	void resizeAndMoveWidget(int _zoomFactorVariation);
	void resizeWidget();
	void zoomOut();
	void zoomIn();
private:
	Ui::ImageDialog ui;
	ImageDialogManager* d_manager;
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

	//Label上左上角第一个点在缩放后的图像上对应的坐标
	int d_imageTopLeftXOnImage;
	int d_imageTopLeftYOnImage;

	//鼠标上的点在Label上显示的图像的坐标
	int d_mousePosToImageLeft;
	int d_mousePosToImageTop;

	//鼠标在对话框上的坐标
	QPoint d_mousePos;

	//鼠标在缩放后的图像上的坐标
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
