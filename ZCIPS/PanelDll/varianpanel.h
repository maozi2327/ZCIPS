#pragma once
#include "panel.h"
#include <tuple>
#include <map>
#include <functional>
#include "../../Public/util/fixedthreadpool.h"
#include <varian/FluoroStructs.h>
#include <varian/HcpErrors.h>
#include <varian/HcpFuncDefs.h>
#include <varian/HcpPxlFormat.h>
#include <varian/HcpSundries.h>
#include <varian/iostatus.h>
#pragma comment(lib, "./public/lib/VirtCp64.lib")

class VarianPanel : public Panel
{
private:
	BinMode d_binMode;
	QString d_dectorType;
	const std::map<BinMode, std::tuple<QString, unsigned short>> d_binModeName;
	unsigned char* d_PESiDetAcqBuffer;
	unsigned char* d_copyBuffer;
	int d_frameCount;
	size_t d_PESiDetBufferSize;
	PESICON_SINGLE_MODE d_PESiContinusSingleMode;
	int d_mode;
	char d_frameReadyName[256];
	SLivePrms* d_liveParams;
	std::atomic<bool> d_grabbingIsActive;
	QWidget* d_widget;
	struct VarianMode
	{
		int bin;
		float pf;
		float vg;
		float fps;
	};
	std::map<int, VarianMode> d_modeBinPfVgFps;
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
	bool d_idle;
	bool d_netWork;
	bool beginAcquire(SampleMode _sampleMode, int _cycleTime, int _frames);
	void imageProcessThread();
	void netWorkThread();
	FixedThreadPool d_fixedThreadPool;
public:
	VarianPanel(QObject *parent);
	~VarianPanel();

	virtual bool connectPanel();
	virtual bool setCycleTime(int _milliseconds);
	virtual bool setFrames(int _frames);
	virtual bool setSampleMode(SampleMode _sampleMode, int _cycleTime);
	bool setVarianMode(int _mode);
	virtual bool setGainFactor(int _gainFactor);
	virtual bool getConnected();
	virtual bool getReady();

	//采集多帧时会将数据连续存储在同一内存区域
	virtual int getSampleTime();
	virtual std::pair<int, int> getPanelSize();
	virtual float getPixelSize();
	virtual bool setPanelSize(int _width, int _height);
	virtual size_t getFrameSize();
	virtual bool beginSoftwareTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, 
		int _frames, int _cycleTime, int _gainFactor);
	virtual bool beginExTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, 
		int _cycleTime, int _gainFactor);
	virtual void stopAcquire();
	virtual void setFrameCallback(std::function<void(unsigned short*)> _imageProcessCallBack);
	virtual int caculateExTriggerSampleTime(int _cycleTime);
	virtual QWidget* getWidget();
};
