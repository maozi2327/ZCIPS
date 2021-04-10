#include "stdafx.h"
#include "PESiPanel.h"
#include "../Public/util/Thread.h"
#include "../Public/util/messagebox.h"

#pragma comment(lib, "./public/lib/xisl.lib")

#define ACQ_SNAP 8
#define SAFE_DELETE(PTR) if(PTR != nullptr){ free(PTR); PTR = nullptr; }
static PESiPanel* ptrPESiPanel;

//TODO_DJ��BinMode��ʼ��
PESiPanel::PESiPanel() 
	: d_binModeName
	(
	{{ BinMode::BinMode1, {QString("1x1"), 1}},
	 { BinMode::BinMode2, {QString("1x1"), 1}},
	 { BinMode::BinMode3, {QString("1x1"), 1}},
	 { BinMode::BinMode4, {QString("1x1"), 1}},
	 { BinMode::BinMode5, {QString("1x1"), 1}}}
	)
	, Panel()
	, d_PESiDetBufferSize(0), d_PESiDetAcqBuffer(nullptr), d_frameCount(0), d_sampleMode(SampleMode::wrongTrigger), d_cycleTime(0), d_gainFactor(10)
	, d_connectThread(new Thread(std::bind(&PESiPanel::connectPanel, this), std::ref(d_deadThreadRun))), d_deadThreadRun(true), d_connected(false)
{
	d_widget = new PEsiPanelWidget();
	ptrPESiPanel = this;
	d_connectThread->detach();
	connect(d_widget, &PEsiPanelWidget::setCycleTimeSignal, this, &PESiPanel::setCycleTimeSlot);
	connect(d_widget, &PEsiPanelWidget::setGainFactorSignal, this, &PESiPanel::setGainFactorSlot);
}

PESiPanel::~PESiPanel()
{

}

void OnEndPESiDetFrameCallback(HACQDESC hAcqDesc)
{
	ptrPESiPanel->OnEndPESiDetFrameCallback(hAcqDesc);
}

void OnEndPESiDetAcqCallback(HACQDESC hAcqDesc)
{
	ptrPESiPanel->OnEndPESiDetAcqCallback(hAcqDesc);
}

bool PESiPanel::connectPanel()
{
	while (d_deadThreadRun)
	{
		if (d_connected)
		{
			std::this_thread::sleep_for(std::chrono::seconds(3));
			continue;
		}

		unsigned int uiNumSensors;			// ̽��������
		unsigned int iRet;

		if ((iRet = Acquisition_EnumSensors(&uiNumSensors, 1, FALSE)) != HIS_ALL_OK)
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_EnumSensors", iRet);
			return false;
		}

		ACQDESCPOS Pos = 0;
		HACQDESC hAcqDesc = NULL;

		do
		{
			if ((iRet = Acquisition_GetNextSensor(&Pos, &hAcqDesc)) != HIS_ALL_OK)
			{
				LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_GetNextSensor", iRet);
				return false;
			}
		} while (Pos != 0);

		UINT nChannelType;
		int nChannelNr;

		if ((iRet = Acquisition_GetCommChannel(hAcqDesc, &nChannelType, &nChannelNr)) != HIS_ALL_OK)
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_GetCommChannel", iRet);
			return false;
		}

		if (nChannelType == HIS_BOARD_TYPE_ELTEC_XRD_FGX ||
			nChannelType == HIS_BOARD_TYPE_ELTEC_XRD_FGE_Opto)
		{
			CHwHeaderInfo Info;
			CHwHeaderInfoEx InfoEx; // new Header 1621

			if ((iRet = Acquisition_GetHwHeaderInfoEx(hAcqDesc, &Info, &InfoEx)) == HIS_ALL_OK)
			{
				if (Info.dwHeaderID == 14 && InfoEx.wCameratype >= 1) {
					unsigned short *pOffsetBufferBinning1 = NULL, *pOffsetBufferBinning2 = NULL;
					WORD wBinning = 1;
					int timings = 8;
					// create lists to receive timings for different binning modes
					double* m_pTimingsListBinning1;
					double* m_pTimingsListBinning2;
					m_pTimingsListBinning1 = (double*)malloc(timings * sizeof(double));
					m_pTimingsListBinning2 = (double*)malloc(timings * sizeof(double));

					//  set detector timing and gain
					Acquisition_SetCameraMode(hAcqDesc, 0);
					Acquisition_SetCameraGain(hAcqDesc, 1);
					// set detector to default binning mode
					Acquisition_SetCameraBinningMode(hAcqDesc, wBinning);
					// get int times for selected binning mode
					Acquisition_GetIntTimes(hAcqDesc, m_pTimingsListBinning1, &timings);
				}
			}
		}
		//��ȡ̽�������ͼ����
		LOG_INFO("channel type : %d  ChannelNr : %d", nChannelType, nChannelNr);

		switch (nChannelType)
		{
		case HIS_BOARD_TYPE_ELTEC_XRD_FGE_Opto:
			LOG_INFO("%s%d", "HIS_BOARD_TYPE_ELTEC_XRD_FGE_Opto:", nChannelType);
			break;
		case HIS_BOARD_TYPE_ELTEC_XRD_FGX:
			LOG_INFO("%s%d", "HIS_BOARD_TYPE_ELTEC_XRD_FGX:", nChannelType);
			break;
		case HIS_BOARD_TYPE_ELTEC:
			LOG_INFO("%s%d", "HIS_BOARD_TYPE_ELTEC:", nChannelType);
			break;
		case HIS_BOARD_TYPE_ELTEC_GbIF:
			LOG_INFO("%s%d", "HIS_BOARD_TYPE_ELTEC_GbIF:", nChannelType);
			break;
		case HIS_BOARD_TYPE_ELTEC_EMBEDDED:
			LOG_INFO("%s%d", "HIS_BOARD_TYPE_ELTEC_EMBEDDED:", nChannelType);
			break;
		default:
			LOG_INFO("%s%d", "Unknown ChanelType::", nChannelType);
			break;
		}
		//��ȡ̽��������
		unsigned int width, height;
		if ((iRet = Acquisition_GetConfiguration(hAcqDesc, 0, &width, &height, 0,
			0, 0, 0, 0, 0, 0)) != HIS_ALL_OK)
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_GetConfiguration", iRet);
			return false;
		}

		if (width != d_width || height != d_height)
		{
			messageBox(QString::fromLocal8Bit("����̽�����������ô���"), QString::fromLocal8Bit("����������̽��������������"));
			break;
		}

		d_frameSize = d_width * d_height * sizeof(unsigned short);

		if ((iRet = Acquisition_SetCallbacksAndMessages(hAcqDesc, NULL, 0,
			0, ::OnEndPESiDetFrameCallback, ::OnEndPESiDetAcqCallback)) != HIS_ALL_OK)		//���òɼ��ص�����
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetCallbacksAndMessages", iRet);
			return false;
		}

		if ((iRet = Acquisition_SetCameraTriggerMode(hAcqDesc, 1)) == HIS_ALL_OK)
		{
			LOG_INFO("%sʧ�ܣ�������%d", "Acquisition_GetConfiguration", iRet);
		}
		else
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetCameraTriggerMode", iRet);
			return false;
		}

		d_connected = true;

		hPESiAcqDesc = hAcqDesc;
	}

	return true;
}
void PESiPanel::OnEndPESiDetFrameCallback(HACQDESC hAcqDesc)
{
	QString time = QDateTime::currentDateTime().time().toString();
	QByteArray byte = time.toLocal8Bit();
	printf("%s\n", byte.data());
	//TODO_DJ:���ûص�����Ϊ��ʱ
	d_imageProcCallback((unsigned short*)(d_PESiDetAcqBuffer + d_frameSize * d_frameCount));

	if (d_sampleMode == SampleMode::softTrigger)
	{
		if (d_PESiContinusSingleMode == PESICON_SINGLE_MODE::Continus ||
			d_PESiContinusSingleMode == PESICON_SINGLE_MODE::Single && d_frameCount != d_frames)
			Acquisition_SetFrameSync(hAcqDesc);
	}
}
int PESiPanel::getFramesSet()
{
	return d_widget->ui.singleShotFramesSpinBox->value();
}
int PESiPanel::getSampleTimeSet()
{
	return d_widget->ui.cycleTimeEdit->text().toInt();
}
int PESiPanel::getGainFactorSet()
{
	return d_gainFactor;
}
void PESiPanel::OnEndPESiDetAcqCallback(HACQDESC hAcqDesc)
{

}
bool PESiPanel::getReady()
{
	return false;
}
int PESiPanel::getSampleTime()
{
	return 0;
}
bool PESiPanel::getConnected()
{
	return d_connected;
}
size_t PESiPanel::getFrameSize()
{
	return d_width * d_height * sizeof(unsigned short);
}
bool PESiPanel::setCycleTime(int _milliseconds)
{
	unsigned int iRet;
	d_cycleTime = _milliseconds * 1000;

	if ((iRet = Acquisition_SetFrameSyncTimeMode(hPESiAcqDesc, 0, d_cycleTime)) == HIS_ALL_OK)
	{
		LOG_INFO("�����ӳ�ʱ��:%d", d_cycleTime);
	}
	else 
	{
		LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetCameraMode", iRet);
	}

	return true;
}
bool PESiPanel::setBinMode(BinMode _binMode)
{
	unsigned int iRet;
	unsigned short binFmt;

	auto it = d_binModeName.find(_binMode);
	binFmt = std::get<1>(it->second);

	if ((iRet = Acquisition_SetCameraBinningMode(hPESiAcqDesc, binFmt)) == HIS_ALL_OK)
	{
		LOG_INFO("���úϲ�ģʽ:%d", std::get<1>(it->second));
		d_binMode = _binMode;
		return true;
	}
	else
	{
		LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetCameraBinningMode", iRet);
		return false;
	}

}
bool PESiPanel::setSampleMode(SampleMode _sampleMode, int _cycleTime)
{
	if (d_sampleMode == SampleMode::softTrigger && _sampleMode == SampleMode::softTrigger && d_cycleTime == _cycleTime)
		return true;

	unsigned int iRet;
	QString errorMsg;
	
	//����̽����ΪDDD(Date Delivered on Demand)�޼�϶������ʽ
	if ((iRet = Acquisition_SetCameraTriggerMode(hPESiAcqDesc, 1)) == HIS_ALL_OK)
	{
		if ((iRet = Acquisition_SetFrameSyncTimeMode(hPESiAcqDesc, 0, _cycleTime)) == HIS_ALL_OK)
			LOG_INFO("�����ӳ�ʱ��:%d ms", _cycleTime);
		else
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetCameraBinningMode", iRet);
			return false;
		}
		
		//�����ⴥ���źŸ�ʽ
		if ((iRet = Acquisition_SetTriggerOutSignalOptions(hPESiAcqDesc,
			4 /* 4 - DDD_Pulse*/,
			0, 0, 0, 0, 0,
			_cycleTime,	//�ӳ�ʱ��
			0, 0)) == HIS_ALL_OK)
		{
			LOG_INFO("����TriggerOut�ӳ�:%d ms", _cycleTime);
			d_cycleTime = _cycleTime;
		}
		else 
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetTriggerOutSignalOptions", iRet);
			return false;
		}

		if ((iRet = Acquisition_SetFrameSyncMode(hPESiAcqDesc, d_sampleModeMap[_sampleMode])) != HIS_ALL_OK)
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetTriggerOutSignalOptions", iRet);
			return false;
		}

		d_sampleMode = _sampleMode;
	}
	else 
	{
		LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetCameraTriggerMode", iRet);
		return false;
	}

	return true;
}

bool PESiPanel::setGainFactor(int _gainFactor)
{
	unsigned short iRet;
	//TODO�����δ����ʱ������ΪhPESiAcqDesc�������������쳣
	if (_gainFactor == d_gainFactor)
		return true;

	if ((iRet = Acquisition_SetCameraGain(hPESiAcqDesc, _gainFactor)) == HIS_ALL_OK)
	{
		LOG_INFO("����̽������:%d", _gainFactor);
		d_gainFactor = _gainFactor;
		return true;
	}
	else
	{
		LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetCameraGain", iRet);
		return false;
	}
}
//TODO_DJ���ص���Ƭ�Ի������Ȼ�ڲɼ�
//TODO_DJ:������βɼ�������ڴ�ܴ����ڲɼ���ɻ���ֹͣ�ɼ������d_PESiDetAcqBuffer
bool PESiPanel::beginSoftwareTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _frames, 
	int _cycleTime, int _gainFactor)
{
	setFrameCallback(_imageProcessCallBack);
	d_PESiContinusSingleMode = PESICON_SINGLE_MODE::Single;
	return beginAcquire(SampleMode::softTrigger, _cycleTime, _frames);
}

bool PESiPanel::beginPreview(std::function<void(unsigned short*)> _imageProcessCallBack, int _cycleTime, int _gainFactor)
{
	setFrameCallback(_imageProcessCallBack);
	d_PESiContinusSingleMode = PESICON_SINGLE_MODE::Continus;
	return beginAcquire(SampleMode::softTrigger, _cycleTime, 1, true);
}
bool PESiPanel::beginExTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _cycleTime, 
	int _gainFactor)
{
	setFrameCallback(_imageProcessCallBack);
	d_PESiContinusSingleMode = PESICON_SINGLE_MODE::Single;
	return beginAcquire(SampleMode::exTrigger, _cycleTime, 1);
}

void PESiPanel::stopAcquire()
{
	//����ɨ��ǰ���øú�����������尴��֮ǰ��ģʽ����ɨ��
	if(d_sampleMode == SampleMode::exTrigger)
		Acquisition_Abort(hPESiAcqDesc);
	//Acquisition_SetFrameSyncMode(hPESiAcqDesc, HIS_SYNCMODE_FREE_RUNNING);
	//d_sampleMode = SampleMode::freeRunning;
}

bool PESiPanel::setFrames(int _frames)
{
	d_frames = _frames;
	return true;
}

float PESiPanel::getPixelSize()
{
	return d_pixelSize;
}

void PESiPanel::setFrameCallback(std::function<void(unsigned short*)> _imageProcessCallBack)
{
	d_imageProcCallback = _imageProcessCallBack;
}

int PESiPanel::caculateExTriggerSampleTime(int _cycleTime)
{
	return _cycleTime + d_exTriggerTimeMargin;
}

std::pair<int, int> PESiPanel::getPanelSize()
{
	return std::pair<int, int>{ d_width, d_height };
}

bool PESiPanel::setPanelSize(int _width, int _height)
{
	d_width = _width;
	d_height = _height;
	d_frameSize = d_width * d_height * sizeof(unsigned short);
	return true;
}

QWidget * PESiPanel::getWidget()
{
	return d_widget;
}

void PESiPanel::setCycleTimeSlot(int _milliseconds)
{
	setCycleTime(_milliseconds);
}

void PESiPanel::setGainFactorSlot(QString _text)
{
	int gain = 2;

	if (_text == "0.25pF")
		gain = 0;
	else if (_text == "0.5pF")
		gain = 1;
	else if (_text == "1pF")
		gain = 2;
	else if (_text == "2pF")
		gain = 3;
	else if (_text == "4pF")
		gain = 4;
	else if (_text == "8pF")
		gain = 5;

	setGainFactor(gain);
}
void PESiPanel::detectorConnectedSlot()
{
	
}

bool PESiPanel::beginAcquire(SampleMode _sampleMode, int _cycleTime, int _frames, bool realTime)
{
	unsigned int iRet;
	unsigned long dwDemoParam = ACQ_SNAP;
	d_frameCount = 0;

	if(!setSampleMode(_sampleMode, _cycleTime))
		return false;

	if (Acquisition_SetAcqData(hPESiAcqDesc, (void*)&dwDemoParam) != HIS_ALL_OK)
	{
		LOG_INFO("Acquisition_SetAcqData ʧ�ܣ�");
		return false;
	}

	d_frames = _frames;

	//����ɼ��ڴ�,���û�����
	if (d_frameSize * d_frames != d_PESiDetBufferSize)
	{
		if (d_PESiDetAcqBuffer != nullptr)
			SAFE_DELETE(d_PESiDetAcqBuffer);

		d_PESiDetAcqBuffer = (unsigned char*)malloc(d_frameSize * d_frames);
		d_PESiDetBufferSize = d_frameSize;
		iRet = Acquisition_DefineDestBuffers(hPESiAcqDesc, (unsigned short*)d_PESiDetAcqBuffer, _frames,
			d_height, d_width);
		
		if (iRet != HIS_ALL_OK)
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_DefineDestBuffers", iRet);
			return false;
		}
	}

	unsigned int bHisSeqFlag;

	if(_sampleMode == SampleMode::exTrigger)
		bHisSeqFlag = HIS_SEQ_CONTINUOUS;
	else if (_sampleMode == SampleMode::softTrigger)
	{
		if(realTime)
			bHisSeqFlag = HIS_SEQ_CONTINUOUS;
		else
			bHisSeqFlag = HIS_SEQ_ONE_BUFFER;
	}

	//NOTEPAD
	//ʹ��HIS_SEQ_CONTINUOUS�ⴥ��ʱ����ֹͣ�����������
	//Acquisition_Abort(hPESiAcqDesc)���Բ�ͬ�ڲ���ʱ����ٶȼ����ɼ�ͼ��
	//ʹ��HIS_SEQ_ONE_BUFFER�ⴥ��ʱֻ�ܲɼ�һ��ͼ��
	//NOTEPAD
	Acquisition_ResetFrameCnt(hPESiAcqDesc);						//֡������
	iRet = Acquisition_Acquire_Image(hPESiAcqDesc, d_frames, 0, bHisSeqFlag, 0, 0, 0);

	if (iRet != HIS_ALL_OK)
	{
		LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_Acquire_Image", iRet);
		return false;
	}

	if (d_sampleMode == SampleMode::softTrigger) {
		iRet = Acquisition_SetFrameSync(hPESiAcqDesc);

		if (iRet != HIS_ALL_OK)
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetFrameSync", iRet);
			return false;
		}
	}

	return true;
}
