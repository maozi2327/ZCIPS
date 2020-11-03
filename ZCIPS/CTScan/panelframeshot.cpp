#include "stdafx.h"
#include "panelframeshot.h"
#include "../PanelDll/panel.h"

PanelFrameShot::PanelFrameShot(Panel* _panel, QObject *parent)
	: QObject(parent), d_panel(_panel)
{

}

PanelFrameShot::~PanelFrameShot()
{
}

void PanelFrameShot::frameShotCallback(unsigned short * _image)
{
}

void PanelFrameShot::previewCallback(unsigned short * _image)
{
}

void PanelFrameShot::beginAcquire(int _frames, int _cycleTime)
{
	std::function<void(unsigned short*)> callBack = std::bind(
		&PanelFrameShot::frameShotCallback, this, std::placeholders::_1);
	d_panel->setFrameCallback(callBack);
	d_panel->beginSoftwareTriggerAcquire(callBack, _frames, _cycleTime);
}

void PanelFrameShot::beginPreview(int _cycleTime)
{
	std::function<void(unsigned short*)> callBack = std::bind(
		&PanelFrameShot::previewCallback, this, std::placeholders::_1);
	d_panel->setFrameCallback(callBack);
}
