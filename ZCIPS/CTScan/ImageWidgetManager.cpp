#include "stdafx.h"
#include "imagewidgetmanager.h"
#include "imagewidget.h"


ImageWidgetManager::ImageWidgetManager()
{

}


ImageWidgetManager::~ImageWidgetManager()
{
}
bool ImageWidgetManager::showImageInNewWindow(unsigned char* _imageMemory, int _width, int _height)
{
	d_imageWidgetVec.push_back(new ImageWidget(this, 0, 0, _imageMemory, _width, _height));
	auto imageWidget = d_imageWidgetVec.back();
	imageWidget->show();
	return true;
}
bool ImageWidgetManager::showImageInFrontWindow(unsigned char* _imageMemory, int _width, int _height)
{
	d_imageWidgetVec.push_back(new ImageWidget(this, 0, 0, _imageMemory, _width, _height));
	d_imageWidgetVec[d_frontWidgetIndex]->loadImage(_imageMemory, _width, _height);
	return true;
}
bool ImageWidgetManager::showImageInNewWindow(QString& _fileName)
{
	d_imageWidgetVec.push_back(new ImageWidget(this, 0, 0, _fileName));
	auto imageWidget = d_imageWidgetVec.back();
	imageWidget->show();
	return true;
}
bool ImageWidgetManager::showImageInFrontWindow(QString& _fileName)
{
	d_imageWidgetVec[d_frontWidgetIndex]->loadImage(_fileName);
	return true;
}
