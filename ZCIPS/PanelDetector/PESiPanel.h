#pragma once
#include "Panel.h"
#include "Acq.h"
#include "panelHeaders.h"
#include "../Public/headers/CommandQueue.h"
#include <tuple>
#include <map>
#include <functional>
#include "pesipanelwidget.h"
class PESiPanel : public Panel
{
private:
	BinMode d_binMode;
	QString d_dectorType;
	HACQDESC hPESiAcqDesc;
	const std::map<BinMode, std::tuple<QString, unsigned short>> d_binModeName;
	unsigned char* d_PESiDetAcqBuffer;
	unsigned char* d_copyBuffer;
	int d_frameCount;
	size_t d_PESiDetBufferSize;
	PESICON_SINGLE_MODE d_PESiContinusSingleMode;
	PEsiPanelWidget* d_widget;
protected:
	std::function<void(unsigned short*)> d_imageProcCallback;
	unsigned int d_width;
	unsigned int d_height;
	int d_depth;
	int d_frames;
	int d_cycleTime;
	int d_sampleTime;
	const int d_exTriggerTimeMargin = 200;
	int d_gainFactor;
	size_t d_frameSize;
	SampleMode d_sampleMode;
	float d_pixelSize;

public:
	PESiPanel();
	~PESiPanel();
	virtual bool setFrames(int _frames) override;
	virtual void stopAcquire() override;
	virtual bool connectPanel() override;
	virtual bool beginSoftwareTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _frames, int _cycleTime, int _gainFactor) override;
	virtual bool beginExTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _cycleTime, int _gainFactor) override;
	virtual bool setBinMode(BinMode _binMode);
	virtual bool setCycleTime(int _milliseconds) override;
	virtual bool setSampleMode(SampleMode _sampleMode, int _cycleTime) override;
	virtual bool setGainFactor(int _gainFactor) override;
	virtual bool getReady() override;
	virtual int getSampleTime() override;
	virtual bool getConnected() override;
	virtual size_t getFrameSize() override;
	virtual float getPixelSize() override;
	virtual void setFrameCallback(std::function<void(unsigned short*)> _imageProcessCallBack) override;
	virtual int caculateExTriggerSampleTime(int _cycleTime) override;
	virtual std::pair<int, int> getPanelSize() override;
	virtual bool setPanelSize(int _width, int _height) override;
	virtual QWidget* getWidget() override;
	virtual void OnEndPESiDetAcqCallback(HACQDESC hAcqDesc);
	virtual void OnEndPESiDetFrameCallback(HACQDESC hAcqDesc);			//声明每帧图像采集回调函数
public slots:
	void setCycleTimeSlot(int _milliseconds);
	void setGainFactorSlot(QString _text);

protected:
	bool beginAcquire(SampleMode _sampleMode, int _cycleTime, int _frames);
};

