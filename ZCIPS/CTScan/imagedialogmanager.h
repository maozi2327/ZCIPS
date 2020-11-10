#pragma once
#include <vector>
#include <memory>

class ImageDialog;
class ImageDialogManager
{
public:
	ImageDialogManager();
	~ImageDialogManager();

	bool showImageInNewWindow(unsigned char* _imageMemory, int _width, int _height);
	bool showImageInCurrentWindow(unsigned char* _imageMemory, int _width, int _height);
	bool showImageInNewWindow(QString& _fileName);
	bool showImageInCurrentWindow(QString& _fileName);
private:
	QString d_rootDirectory;
	std::vector<ImageDialog*> d_imageDialogVec;
	std::size_t d_frontWidgetIndex;
	int d_screenWidth;
	int d_screenHeight;
};

