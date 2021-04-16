#pragma once

#include <QObject>
#include "paneldll_global.h"
#include <memory>
#include "../Public/util/logmacro.h"
#include <functional>

//#define NIMAQPANEL
//#ifdef NIMAQPANEL
//#define REALPANEL NimaqPanel
//#else
//#define REALPANEL PESiPanel
//#endif

enum SampleMode
{
	softTrigger = 1,
	exTrigger = 2,
	freeRunning = 4,
	wrongTrigger = 10
};
enum class BinMode
{
	BinMode1,
	BinMode2,
	BinMode3,
	BinMode4,
	BinMode5,
	BinMode6,
};
enum class PESICON_SINGLE_MODE
{
	Continus,
	Single
};

class Panel	: public QObject
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
	virtual bool beginPreview(std::function<void(unsigned short*)> _imageProcessCallBack, int _cycleTime,
		int _gainFactor) = 0;
	virtual bool beginSoftwareTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _frames
		, int _cycleTime, int _gainFactor) = 0;
	virtual bool beginExTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _cycleTime
		, int _gainFactor) = 0;
	virtual void stopAcquire() = 0;
	virtual void setFrameCallback(std::function<void(unsigned short*)> _imageProcessCallBack) =  0;
	virtual int caculateExTriggerSampleTime(int _cycleTime) = 0;
	virtual QWidget* getWidget() = 0;
	virtual QWidget* getTempWidget() = 0;
	virtual int getTempGainFactorSet() = 0;
	virtual int getTempSampleTimeSet() = 0;
	virtual int getSampleTimeSet() = 0;
	virtual int getGainFactorSet() = 0;
};
class PANELDLL_EXPORT PanelFactory
{
public:
	static std::unique_ptr<Panel> getPanel(int _type);
};