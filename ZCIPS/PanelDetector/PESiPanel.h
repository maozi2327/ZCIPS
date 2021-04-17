#pragma once
#include "Panel.h"
#include ".\public\headers\Acq.h"
#include <tuple>
#include <map>
#include "pesipanelwidget.h"
class Thread;

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
	PEsiPanelWidget* d_tempWidget;
	std::map<SampleMode, unsigned long> d_sampleModeMap{ {SampleMode::softTrigger, 1},  {SampleMode::exTrigger, 3} };
protected:
	std::function<void(unsigned short*)> d_imageProcCallback;
	unsigned int d_width;
	unsigned int d_height;
	int d_depth;
	int d_frames;
	int d_cycleTime;
	int d_sampleTime;
	const int d_exTriggerTimeMargin = 270 * 2;
	int d_gainFactor;
	size_t d_frameSize;
	SampleMode d_sampleMode;
	float d_pixelSize;
	std::unique_ptr<Thread> d_connectThread;
	std::atomic<bool> d_deadThreadRun;
	bool d_connected;
public:
	PESiPanel();
	~PESiPanel();
	virtual bool setFrames(int _frames);
	virtual void stopAcquire();
	virtual bool connectPanel();
	virtual bool beginSoftwareTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _frames, int _cycleTime, 
		int _gainFactor);
	virtual bool beginPreview(std::function<void(unsigned short*)> _imageProcessCallBack, int _cycleTime, int _gainFactor);
	virtual bool beginExTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _cycleTime, int _gainFactor);
	virtual bool setBinMode(BinMode _binMode);
	virtual bool setCycleTime(int _milliseconds);
	virtual bool setSampleMode(SampleMode _sampleMode, int _cycleTime);
	virtual bool setGainFactor(int _gainFactor);
	virtual bool getReady();
	virtual int getSampleTime();
	virtual bool getConnected();
	virtual size_t getFrameSize();
	virtual float getPixelSize();
	virtual void setFrameCallback(std::function<void(unsigned short*)> _imageProcessCallBack);
	virtual int caculateExTriggerSampleTime(int _cycleTime);
	virtual std::pair<int, int> getPanelSize();
	virtual bool setPanelSize(int _width, int _height);
	virtual QWidget* getWidget();
	virtual QWidget* getTempWidget();
	virtual void OnEndPESiDetAcqCallback(HACQDESC hAcqDesc);
	virtual void OnEndPESiDetFrameCallback(HACQDESC hAcqDesc);			//声明每帧图像采集回调函数
	virtual int getSampleTimeSet();
	virtual int getGainFactorSet();
	virtual int getTempGainFactorSet();
	virtual int getTempSampleTimeSet();
	virtual QWidget* getTempShowWidget();
	virtual void setShowWidget(QWidget* _widget, int _gainFactorIndex, int _sampleTime);

public slots:
	void setCycleTimeSlot(int _milliseconds);
	void setGainFactorSlot(QString _text);

private slots:
	void detectorConnectedSlot();
signals:
	void detectorConnectedSignal();
protected:
	bool beginAcquire(SampleMode _sampleMode, int _cycleTime, int _frames, bool realTime = false);
};

