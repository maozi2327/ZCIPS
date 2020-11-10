#include "stdafx.h"
#include "imagedialogmanager.h"
#include "imagedialog.h"
#include <QScreen>

ImageDialogManager::ImageDialogManager()
{
	QScreen *screen = QGuiApplication::primaryScreen();
	QRect screenGeometry = screen->geometry(); 
	d_screenHeight = screenGeometry.height(); 
	d_screenWidth = screenGeometry.width();
}


ImageDialogManager::~ImageDialogManager()
{
}
bool ImageDialogManager::showImageInNewWindow(unsigned char* _imageMemory, int _width, int _height)
{
	d_imageDialogVec.push_back(new ImageDialog(this, d_screenWidth, d_screenHeight, _imageMemory, _width, _height));
	auto imageDialog = d_imageDialogVec.back();
	imageDialog->show();
	return true;
}
bool ImageDialogManager::showImageInCurrentWindow(unsigned char* _imageMemory, int _width, int _height)
{
	if (d_imageDialogVec.size() == 0)
	{
		d_imageDialogVec.push_back(new ImageDialog(this, d_screenWidth, d_screenHeight, _imageMemory, _width, _height));
		d_frontWidgetIndex = 0; 
	}
	else
	{
		d_imageDialogVec[d_frontWidgetIndex]->loadImage(_imageMemory, _width, _height);
	}

	auto front = d_imageDialogVec[d_frontWidgetIndex];
	front->show();
	return true;
}
bool ImageDialogManager::showImageInNewWindow(QString& _fileName)
{
	d_imageDialogVec.push_back(new ImageDialog(this, d_screenWidth, d_screenHeight, _fileName));
	auto imageDialog = d_imageDialogVec.back();
	imageDialog->show();
	return true;
}
bool ImageDialogManager::showImageInCurrentWindow(QString& _fileName)
{
	d_imageDialogVec[d_frontWidgetIndex]->loadImage(_fileName);
	return true;
}
