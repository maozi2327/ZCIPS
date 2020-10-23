//#include "PanelInterface.h"
//
//PanelInterface::PanelInterface(): Panel()
//{
//
//}
//
//
//PanelInterface::~PanelInterface()
//{
//
//}
//
//bool PanelInterface::setFrames(int _frames)
//{
//	d_frames = _frames;
//	d_frameSize = d_width * d_height * d_frames * sizeof(unsigned short);
//	return true;
//}
//size_t PanelInterface::getFrameSize()
//{
//	return d_frameSize;
//}
//float PanelInterface::getPixelSize()
//{
//	return d_pixelSize;
//}
//void PanelInterface::setFrameCallback(std::function<void(unsigned short*)> _imageProcessCallBack)
//{
//	d_imageProcCallback = _imageProcessCallBack;
//}
//int PanelInterface::getSampleTime()
//{
//	return d_sampleTime;
//}
//bool PanelInterface::getPanelSize(int& _width, int& _height)
//{
//	_width = d_width;
//	_height = d_height;
//	return true;
//}
//
//bool PanelInterface::setPanelSize(int _width, int _height)
//{
//	d_width = _width;
//	d_height = _height;
//	return true;
//}