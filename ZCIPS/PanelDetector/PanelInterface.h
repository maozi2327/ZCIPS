//#pragma once
//#pragma comment(lib, "./public/lib/xisl.lib")
//
//#include <qstring.h>
//#include <vector>
//#include "panelHeaders.h"
//#include "panel.h"
//#include "../Public/headers/CommandQueue.h"
//
////���ִֻ�вɼ�������ԭʼ���ݣ������������ļ����ļ��������ļ�����
//class PanelInterface : public Panel
//{
//public:
//    PanelInterface();
//    virtual ~PanelInterface();
//
//protected:
//	std::function<void(unsigned short*)> d_imageProcCallback;
//    unsigned int d_width;
//    unsigned int d_height;
//    int d_depth;
//    int d_frames;
//    int d_sampleTime;
//    int d_gainFactor;
//	size_t d_frameSize;
//    SampleMode d_sampleMode;
//	float d_pixelSize;
//
//public:
//    virtual bool setSampleTime(int _milliseconds) = 0;
//    virtual bool setFrames(int _frames);
//    virtual bool setSampleMode(SampleMode _sampleMode) = 0;
//    virtual bool setGainFactor(int _gainFactor) = 0;
//	virtual bool setBinMode(BinMode _binMode) = 0;
//	virtual bool beginAcquire(unsigned short d_quantity) = 0;
//	virtual void stopAcquire() = 0;
//	virtual bool initialise() = 0;
//    //�ɼ���֡ʱ�Ὣ���������洢��ͬһ�ڴ�����
//	virtual int getSampleTime();
//    virtual bool getPanelSize(int& _width, int& _height);
//    virtual bool setPanelSize(int _width, int _height);
//    virtual size_t getFrameSize();
//	virtual float getPixelSize();
//	virtual void setFrameCallback(std::function<void(unsigned short*)> _imageProcessCallBack);
//};