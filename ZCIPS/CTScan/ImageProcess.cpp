#include "stdafx.h"
#include "imageprocess.h"

ImageProcess::ImageProcess()
{
}


ImageProcess::~ImageProcess()
{
}

void ImageProcess::rotation90(unsigned short* _image, int _width, int _height, int _frames)
{
	size_t imageSize = _width * _height * sizeof(unsigned short);
	unsigned short* tempMemory = (unsigned short*)malloc(imageSize);
	memcpy(tempMemory, _image, imageSize);
	int newWidth = _height;
	int newHeight = _width;
	
	for(int i = 0; i != newHeight; ++i )
		for (int j = 0; j != newWidth; ++j)
			*(_image + i * newWidth + j) = *(tempMemory + (_height - j - 1) * _width + i);

	free(tempMemory);
}
void ImageProcess::rotation180(unsigned short* _image, int _width, int _height, int _frames)
{
	size_t imageSize = _width * _height * sizeof(unsigned short);
	unsigned short* tempMemory = (unsigned short*)malloc(imageSize);
	memcpy(tempMemory, _image, imageSize);

	for (int i = 0; i != _height; ++i)
		for (int j = 0; j != _width; ++j)
			*(_image + i * _height + j) = *(tempMemory + (_width - i - 1) * _height + _height - j - 1);

	free(tempMemory);
}
void ImageProcess::rotation270(unsigned short* _image, int _width, int _height, int _frames)
{
	size_t imageSize = _width * _height * sizeof(unsigned short);
	unsigned short* tempMemory = (unsigned short*)malloc(imageSize);
	memcpy(tempMemory, _image, imageSize);
	int newWidth = _height;
	int newHeight = _width;

	for (int i = 0; i != newHeight; ++i)
		for (int j = 0; j != newWidth; ++j)
			*(_image + i * newWidth + j) = *(tempMemory + (_height - j - 1) * _width + _width - i - 1); 

	free(tempMemory);
}
void ImageProcess::upDownReverse(unsigned short* _image, int _width, int _height, int _frames)
{
	size_t imageSize = _width * _height * sizeof(unsigned short);
	unsigned short* tempMemory = (unsigned short*)malloc(imageSize);
	memcpy(tempMemory, _image, imageSize);
	size_t sizePerLine = _width * sizeof(unsigned short);

	for (int i = 0; i != _height; ++i)
		memcpy(_image + i * sizePerLine, tempMemory + (_height - i - 1) * sizePerLine, sizePerLine);

	free(tempMemory);
}
void ImageProcess::leftRightReverse(unsigned short* _image, int _width, int _height, int _frames)
{
	size_t imageSize = _width * _height * sizeof(unsigned short);
	unsigned short* tempMemory = (unsigned short*)malloc(imageSize);
	memcpy(tempMemory, _image, imageSize);
	
	for (int i = 0; i != _height; ++i)
		for (int j = 0; j != _width; ++j)
			*(_image + i * _width * sizeof(unsigned short) + j) = *(_image + i * _width * sizeof(unsigned short) + _width - j - 1);

	free(tempMemory);
}
void ImageProcess::rotation90AndSave(unsigned short* _image, int _width, int _height, QString _fileName, int _frames) 
{

}
void ImageProcess::rotation180AndSave(unsigned short* _image, int _width, int _height, QString _fileName, int _frames)
{

}

void ImageProcess::rotation270AndSave(unsigned short* _image, int _width, int _height, QString _fileName, int _frames)
{

}

void ImageProcess::upDownReverseAndSave(unsigned short* _image, int _width, int _height, QString _fileName, int _frames)
{

}

void ImageProcess::leftRightReverseAndSave(unsigned short* _image, int _width, int _height, QString _fileName, int _frames)
{

}

