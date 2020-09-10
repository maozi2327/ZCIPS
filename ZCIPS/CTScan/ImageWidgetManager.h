#pragma once
#include "imagewidget.h"
#include <vector>
#include <memory>
class ImageWidgetManager
{
public:
	ImageWidgetManager();
	~ImageWidgetManager();

	bool showImageInNewWindow(unsigned char* _imageMemory, int _width, int _height);
	bool showImageInFrontWindow(unsigned char* _imageMemory, int _width, int _height);
	bool showImageInNewWindow(QString& _fileName);
	bool showImageInFrontWindow(QString& _fileName);
private:
	QString d_rootDirectory;
	std::vector<ImageWidget*> d_imageWidgetVec;
	std::size_t d_frontWidgetIndex;
};

