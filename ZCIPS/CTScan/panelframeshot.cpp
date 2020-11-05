#include "stdafx.h"
#include "panelframeshot.h"
#include "../PanelDll/panel.h"
#include "../PanelImageProcess/panelimageprocess.h"

PanelFrameShot::PanelFrameShot(Panel* _panel, PanelImageProcess* _panelImageProcess, QObject *parent)
	: QObject(parent), d_panel(_panel), d_imageProcess(_panelImageProcess)
	, d_frames(0), d_frameCount(0)
{
	d_panel->getPanelSize(d_width, d_height);
}

PanelFrameShot::~PanelFrameShot()
{
}

void PanelFrameShot::frameShotCallback(unsigned short * _image)
{
	d_imageProcess->saveSingleBitmapDataToFile(_image, QString(""), d_height, d_width);
	emit imageAcquiredSignal(_image, d_width, d_height);
	//free(_image);

	//NOTEPAD
	//导致下一次采集程序响应很慢Acquisition_Acquire_Image
	//NOTEPAD
	//if (++d_frameCount == d_frames)
	//	d_panel->stopAcquire();
}

void PanelFrameShot::previewCallback(unsigned short * _image)
{
}

void PanelFrameShot::beginAcquire(int _frames, int _cycleTime, unsigned short _gainFactor)
{
	d_frames = _frames;
	d_frameCount = 0;
	std::function<void(unsigned short*)> callBack = std::bind(
		&PanelFrameShot::frameShotCallback, this, std::placeholders::_1);
	d_panel->setFrameCallback(callBack);
	d_panel->beginSoftwareTriggerAcquire(callBack, _frames, _cycleTime, _gainFactor);
}

void PanelFrameShot::beginPreview(int _cycleTime)
{
	std::function<void(unsigned short*)> callBack = std::bind(
		&PanelFrameShot::previewCallback, this, std::placeholders::_1);
	d_panel->setFrameCallback(callBack);
}
