#pragma once

#include <QObject>
#include "paneldll_global.h"
#include "panelHeaders.h"
#include <memory>
#include "../Public/util/logmacro.h"
#include <functional>
class PANELDLL_EXPORT Panel	: public QObject
{
	Q_OBJECT
signals:
	LOGSIGNAL
public:
    Panel();
	virtual ~Panel() = 0;
	virtual bool connectPanel() = 0;
	virtual bool setCycleTime(int _milliseconds) = 0;
	virtual bool setFrames(int _frames) = 0;
	virtual bool setSampleMode(SampleMode _sampleMode, int _cycleTime) = 0;
	virtual bool setGainFactor(int _gainFactor) = 0;
	virtual bool getConnected() = 0;
	virtual bool getReady() = 0;

	//采集多帧时会将数据连续存储在同一内存区域
	virtual int getSampleTime() = 0;
	virtual std::pair<int, int> getPanelSize() = 0;
	virtual float getPixelSize() = 0;
	virtual bool setPanelSize(int _width, int _height) = 0;
	virtual size_t getFrameSize() = 0;
	virtual bool beginSoftwareTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _frames, int _cycleTime, int _gainFactor) = 0;
	virtual bool beginExTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _cycleTime, int _gainFactor) = 0;
	virtual void stopAcquire() = 0;
	virtual void setFrameCallback(std::function<void(unsigned short*)> _imageProcessCallBack) =  0;
	virtual int caculateExTriggerSampleTime(int _cycleTime) = 0;
	virtual QWidget* getWidget() = 0;
};
class PANELDLL_EXPORT PanelFactory
{
public:
	static std::unique_ptr<Panel> getPanel(int _type);
};