#pragma once
class ImageProcess
{
public:
	ImageProcess();
	~ImageProcess();

	void rotation90(unsigned short* _image, int _width, int _height, int _frames);
	void rotation180(unsigned short* _image, int _width, int _height, int _frames);
	void rotation270(unsigned short* _image, int _width, int _height, int _frames);
	void upDownReverse(unsigned short* _image, int _width, int _height, int _frames);
	void leftRightReverse(unsigned short* _image, int _width, int _height, int _frames);
	void rotation90AndSave(unsigned short* _image, int _width, int _height, QString _fileName, int _frames);
	void rotation180AndSave(unsigned short* _image, int _width, int _height, QString _fileName, int _frames);
	void rotation270AndSave(unsigned short* _image, int _width, int _height, QString _fileName, int _frames);
	void upDownReverseAndSave(unsigned short* _image, int _width, int _height, QString _fileName, int _frames);
	void leftRightReverseAndSave(unsigned short* _image, int _width, int _height, QString _fileName, int _frames);
};

