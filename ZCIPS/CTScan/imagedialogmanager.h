#pragma once
#include <set>
#include <memory>

#define IMAGEDIALOGHANDLE void*

class ImageDialog;
class ImageDialogManager
{
public:
	ImageDialogManager();
	~ImageDialogManager();

	bool showImageInNewWindow(unsigned char* _imageMemory, int _width, int _height);
	bool showImageInNewWindow(QString& _fileName);
	bool showImageInCurrentWindow(IMAGEDIALOGHANDLE _handle, unsigned char* _imageMemory, int _width, int _height);
	bool showImageInCurrentWindow(IMAGEDIALOGHANDLE _handle, QString& _fileName);
	IMAGEDIALOGHANDLE getNewWindow(int _width, int _height);
private:
	QString d_rootDirectory;
	std::set<ImageDialog*> d_imageDialogVec;
	ImageDialog* d_currentImageDialog;
	std::size_t d_frontWidgetIndex;
	int d_screenWidth;
	int d_screenHeight;
};

