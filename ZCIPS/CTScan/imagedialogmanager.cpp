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
	for (auto it : d_imageDialogVec)
		delete it;
}

bool ImageDialogManager::showImageInNewWindow(unsigned char* _imageMemory, int _width, int _height)
{
	auto dialog = new ImageDialog(d_screenWidth, d_screenHeight, _imageMemory, _width, _height);
	d_imageDialogVec.insert(dialog);
	d_currentImageDialog = dialog;
	dialog->show();
	return true;
}

bool ImageDialogManager::showImageInNewWindow(QString& _fileName)
{
	auto dialog = new ImageDialog(d_screenWidth, d_screenHeight, _fileName);
	d_imageDialogVec.insert(dialog);
	d_currentImageDialog = dialog;
	dialog->show();
	return true;
}

bool ImageDialogManager::showImageInCurrentWindow(IMAGEDIALOGHANDLE _handle, unsigned char* _imageMemory, int _width, int _height)
{
	auto ptr = static_cast<ImageDialog*>(_handle);

	if (d_imageDialogVec.find(ptr) != d_imageDialogVec.end())
	{
		ptr->loadImage(_imageMemory, _width, _height);
		ptr->show();
		return true;
	}
	
	return false;
}

bool ImageDialogManager::showImageInCurrentWindow(IMAGEDIALOGHANDLE _handle, QString& _fileName)
{
	return false;
}

IMAGEDIALOGHANDLE ImageDialogManager::getNewWindow(int _width, int _height)
{
	auto dialog = new ImageDialog(d_screenWidth, d_screenHeight, _width, _height);
	d_imageDialogVec.insert(dialog);
	return static_cast<IMAGEDIALOGHANDLE>(dialog);
}
