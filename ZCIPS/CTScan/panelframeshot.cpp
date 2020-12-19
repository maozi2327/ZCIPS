#include "stdafx.h"
#include "panelframeshot.h"
#include "../PanelDetector/panel.h"
#include "../PanelImageProcess/panelimageprocess.h"

PanelFrameShot::PanelFrameShot(Panel* _panel, PanelImageProcess* _panelImageProcess, QObject *parent)
	: QObject(parent), d_panel(_panel), d_imageProcess(_panelImageProcess)
	, d_frames(0), d_frameCount(0)
{

}

PanelFrameShot::~PanelFrameShot()
{
}

void PanelFrameShot::frameShotCallback(unsigned short * _image)
{
	auto width = d_panel->getPanelSize().first;
	auto height = d_panel->getPanelSize().second;
	d_imageProcess->saveSingleBitmapDataToFile(_image, QString(""), width, height);
	emit imageAcquiredSignal(_image, width, height);
	//free(_image);

	//NOTEPAD
	//������һ�βɼ�������Ӧ����Acquisition_Acquire_Image
	//NOTEPAD
	//if (++d_frameCount == d_frames)
	//	d_panel->stopAcquire();
}

void PanelFrameShot::previewCallback(unsigned short * _image)
{
}

void PanelFrameShot::beginAcquire(int _frames, int _cycleTime, unsigned short _gainFactor)
{
	//DEBUG������ͼ����������
	emit imageAcquiredSignal(nullptr, 2048, 2048);
	//DEBUG������ͼ����������

	//DEBUG������ͼ�����ű�ע�͵�
	//d_frames = _frames;
	//d_frameCount = 0;
	//std::function<void(unsigned short*)> callBack = std::bind(
	//	&PanelFrameShot::frameShotCallback, this, std::placeholders::_1);
	//d_panel->setFrameCallback(callBack);
	//d_panel->beginSoftwareTriggerAcquire(callBack, _frames, _cycleTime, _gainFactor);
	//DEBUG������ͼ������
}

void PanelFrameShot::beginPreview(int _cycleTime)
{
	std::function<void(unsigned short*)> callBack = std::bind(
		&PanelFrameShot::previewCallback, this, std::placeholders::_1);
	d_panel->setFrameCallback(callBack);
}
