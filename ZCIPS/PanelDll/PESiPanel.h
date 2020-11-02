#pragma once
#include "Panel.h"
#include "Acq.h"
#include "panelHeaders.h"
#include "../Public/headers/CommandQueue.h"
#include <tuple>
#include <map>
#include <functional>
class PESiPanel : public Panel
{
private:
	BinMode d_binMode;
	QString d_dectorType;
	char d_NiImaqintfName[64];
	int d_quantityOfRingBuffer;
	bool d_ringThreadRunFlag;
	bool d_serialSendRunFlag;
	std::mutex d_ringThreadMutex;
	std::condition_variable d_ringThreadCond;
	HACQDESC hPESiAcqDesc;
	const std::map<BinMode, std::tuple<QString, unsigned short>> d_binModeName;
	unsigned short* d_PESiDetAcqBuffer;
	unsigned char* d_copyBuffer;
	int d_frameCount;
	size_t d_PESiDetBufferSize;
	PESICON_SINGLE_MODE d_PESiContinusSingleMode;
protected:
	std::function<void(unsigned short*)> d_imageProcCallback;
	unsigned int d_width;
	unsigned int d_height;
	int d_depth;
	int d_frames;
	int d_cycleTime;
	int d_sampleTime;
	int d_gainFactor;
	size_t d_frameSize;
	SampleMode d_sampleMode;
	float d_pixelSize;
public:
	PESiPanel();
	~PESiPanel();
	virtual bool setFrames(int _frames);
	virtual void stopAcquire();
	virtual bool initialise();
	virtual bool beginAcquire(unsigned short _quantity, int _cycleTime) override;
	virtual bool setBinMode(BinMode _binMode);
	virtual bool setCycleTime(int _milliseconds) override;
	virtual bool setSampleMode(SampleMode _sampleMode, int _cycleTime) override;
	virtual bool setGainFactor(int _gainFactor);
	virtual void OnEndPESiDetFrameCallback(HACQDESC hAcqDesc);			//声明每帧图像采集回调函数
	virtual void OnEndPESiDetAcqCallback(HACQDESC hAcqDesc);
	virtual bool getReady();
	virtual int getSampleTime();
	virtual bool getConnected();
	virtual size_t getFrameSize();
	virtual float getPixelSize();
	virtual void setFrameCallback(std::function<void(unsigned short*)> _imageProcessCallBack);
	virtual bool getPanelSize(int& _width, int& _height);
	virtual bool setPanelSize(int _width, int _height);
};

