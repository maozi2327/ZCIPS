#include "stdafx.h"
#include "imagewidgetmanager.h"
#include "imagewidget.h"
#include <QScreen>

ImageWidgetManager::ImageWidgetManager()
{
	QScreen *screen = QGuiApplication::primaryScreen();
	QRect screenGeometry = screen->geometry(); 
	d_screenHeight = screenGeometry.height(); 
	d_screenWidth = screenGeometry.width();
}


ImageWidgetManager::~ImageWidgetManager()
{
}
bool ImageWidgetManager::showImageInNewWindow(unsigned char* _imageMemory, int _width, int _height)
{
	d_imageWidgetVec.push_back(new ImageWidget(this, d_screenWidth, d_screenHeight, _imageMemory, _width, _height));
	auto imageWidget = d_imageWidgetVec.back();
	imageWidget->show();
	return true;
}
bool ImageWidgetManager::showImageInFrontWindow(unsigned char* _imageMemory, int _width, int _height)
{
	if (d_imageWidgetVec.size() == 0)
	{
		d_imageWidgetVec.push_back(new ImageWidget(this, d_screenWidth, d_screenHeight, _imageMemory, _width, _height));
		d_frontWidgetIndex = 0; 
	}
	else
	{
		d_imageWidgetVec[d_frontWidgetIndex]->loadImage(_imageMemory, _width, _height);
	}

	auto front = d_imageWidgetVec[d_frontWidgetIndex];
	front->show();
	return true;
}
bool ImageWidgetManager::showImageInNewWindow(QString& _fileName)
{
	d_imageWidgetVec.push_back(new ImageWidget(this, d_screenWidth, d_screenHeight, _fileName));
	auto imageWidget = d_imageWidgetVec.back();
	imageWidget->show();
	return true;
}
bool ImageWidgetManager::showImageInFrontWindow(QString& _fileName)
{
	d_imageWidgetVec[d_frontWidgetIndex]->loadImage(_fileName);
	return true;
}
