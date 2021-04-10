#include "stdafx.h"
#include "panelframesamleinterface.h"
#include "../PanelDetector/panel.h"
#include "../PanelImageProcess/panelimageprocess.h"
#include "../../public/util/functions.h"

PanelFrameSamleInterface::PanelFrameSamleInterface(Panel* _panel, PanelImageProcess* _panelImageProcess, QObject *parent)
	: QObject(parent), d_panel(_panel), d_imageProcess(_panelImageProcess)
	, d_frames(0), d_frameCount(0), d_width(_panel->getPanelSize().first), d_height(_panel->getPanelSize().second)
{
	d_imageBuff = new unsigned long[d_width * d_height];
	d_outputBuff = new unsigned short[d_width * d_height];
}

PanelFrameSamleInterface::~PanelFrameSamleInterface()
{
	delete[] d_imageBuff;
	delete[] d_outputBuff;
}

void PanelFrameSamleInterface::saveFile()
{
}

void PanelFrameSamleInterface::frameShotCallback(unsigned short * _image)
{
	for (int i = 0; i != d_height; ++i)
		for (int j = 0; j != d_width; ++j)
			d_imageBuff[i * d_width + j] += _image[i * d_width + j];

	if (++d_frameCount == d_frames)
	{
		d_panel->stopAcquire();

		for (int i = 0; i != d_height; ++i)
			for (int j = 0; j != d_width; ++j)
				d_outputBuff[i * d_width + j] = d_imageBuff[i * d_width + j] / d_frames;

		saveFile();
	}

	
	char mes[255];
	sprintf(mes, "已经采集%d幅，共%d幅", d_frameCount, d_frames);
	QString message = QString::fromLocal8Bit(mes);
	emit frameAcquiredSignal(d_frameCount, d_frames, d_frameCount, d_frames, message);
	//TODO_DJ:分配用于保存多帧图片的内存，将多帧图片保存到内存中
	//emit imageAcquiredSignal(_image, width, height);
	//free(_image);

	//NOTEPAD
	//导致下一次采集程序响应很慢Acquisition_Acquire_Image
	//NOTEPAD
}

void PanelFrameSamleInterface::setFileName(const QString& _orgName, const QString& _destPath)
{
	d_orgName = _orgName;
	d_filePath = _destPath;
}

void PanelFrameSamleInterface::stopAcquire()
{
	d_panel->stopAcquire();
}

void PanelFrameSamleInterface::beginAcquire(int _frames, int _cycleTime, unsigned short _gainFactor)
{
	//DEBUG：测试图像缩放新增
	//emit imageAcquiredSignal(nullptr, 2048, 2048);
	//DEBUG：测试图像缩放新增

	//DEBUG：测试图像缩放被注释掉
	memset(d_imageBuff, 0, sizeof(unsigned long) * d_width * d_height);
	d_frames = _frames;
	d_frameCount = 0;
	std::function<void(unsigned short*)> callBack = std::bind(&PanelFrameSamleInterface::frameShotCallback, this, std::placeholders::_1);
	d_panel->stopAcquire();
	d_panel->beginSoftwareTriggerAcquire(callBack, _frames, _cycleTime, _gainFactor);
	//DEBUG：测试图像缩放
}
