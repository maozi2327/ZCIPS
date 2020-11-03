#include "PESiPanel.h"
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
	, d_PESiDetBufferSize(0), d_PESiDetAcqBuffer(nullptr), d_frameCount(0)
{
	ptrPESiPanel = this;
	initialise();
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

bool PESiPanel::initialise()
{
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
	} 
	while (Pos != 0);
	
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
	if ((iRet = Acquisition_GetConfiguration(hAcqDesc, 0, &d_height, &d_width, 0,
											0, 0, 0, 0, 0, 0)) != HIS_ALL_OK) 
	{
		LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_GetConfiguration", iRet);
		return false;
	}
	
	if ((iRet = Acquisition_SetCallbacksAndMessages(hAcqDesc, NULL, 0,
		0, ::OnEndPESiDetFrameCallback, ::OnEndPESiDetAcqCallback)) != HIS_ALL_OK)		//���òɼ��ص�����
	{
		LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetCallbacksAndMessages", iRet);
		return false;
	}

	hPESiAcqDesc = hAcqDesc;
	return true;
}
void PESiPanel::OnEndPESiDetFrameCallback(HACQDESC hAcqDesc)
{
	if (d_frameCount == 0)
		d_copyBuffer = (unsigned char*)malloc(d_frameSize * d_frames);

	memcpy(d_copyBuffer + d_frameSize * d_frameCount, d_PESiDetAcqBuffer + d_frameSize * d_frameCount, d_frameSize);

	if (++d_frameCount == d_frames)
	{
		d_imageProcCallback((unsigned short*)d_copyBuffer);
		d_frameCount = 0;
	}

	if (d_sampleMode == SampleMode::softTrigger)
	{
		if (d_PESiContinusSingleMode == PESICON_SINGLE_MODE::Continus ||
			d_PESiContinusSingleMode == PESICON_SINGLE_MODE::Single && d_frameCount != d_frames)
			Acquisition_SetFrameSync(hAcqDesc);
	}
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
	return false;
}
size_t PESiPanel::getFrameSize()
{
	return size_t();
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
	unsigned int iRet;
	QString errorMsg;
	d_sampleMode = _sampleMode;
	
	//����̽����ΪDDD(Date Delivered on Demand)�޼�϶������ʽ
	if ((iRet = Acquisition_SetCameraTriggerMode(hPESiAcqDesc, 1)) == HIS_ALL_OK)
	{
		if ((iRet = Acquisition_SetFrameSyncTimeMode(hPESiAcqDesc, 0, d_cycleTime)) == HIS_ALL_OK)
			LOG_INFO("�����ӳ�ʱ��:%d ms", d_cycleTime);
		else
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetCameraBinningMode", iRet);
			return false;
		}
		
		//�����ⴥ���źŸ�ʽ
		if ((iRet = Acquisition_SetTriggerOutSignalOptions(hPESiAcqDesc,
			4 /* 4 - DDD_Pulse*/,
			0, 0, 0, 0, 0,
			d_cycleTime,	//�ӳ�ʱ��
			0, 0)) == HIS_ALL_OK)
		{
			LOG_INFO("����TriggerOut�ӳ�:%d ms", d_cycleTime);
		}
		else 
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetTriggerOutSignalOptions", iRet);
			return false;
		}

		if ((iRet = Acquisition_SetFrameSyncMode(hPESiAcqDesc, unsigned long(d_sampleMode))) == HIS_ALL_OK) 
		{
			LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetTriggerOutSignalOptions", iRet);
		}
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
	unsigned int iRet;
	
	if ((iRet = Acquisition_SetCameraGain(hPESiAcqDesc, d_gainFactor)) == HIS_ALL_OK)
	{
		LOG_INFO("����̽������:%d", d_gainFactor);
		return true;
	}
	else
	{
		LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_SetCameraGain", iRet);
		return false;
	}
}
//TODO_DJ:������βɼ�������ڴ�ܴ����ڲɼ���ɻ���ֹͣ�ɼ������d_PESiDetAcqBuffer
bool PESiPanel::beginSoftwareTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _frames, int _cycleTime)
{
	setSampleMode(SampleMode::softTrigger, _cycleTime);
	setFrameCallback(_imageProcessCallBack);
	d_PESiContinusSingleMode = PESICON_SINGLE_MODE::Single;
	return beginAcquire(SampleMode::softTrigger, _cycleTime, _frames);
}

bool PESiPanel::beginExTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _cycleTime)
{
	setFrameCallback(_imageProcessCallBack);
	d_PESiContinusSingleMode = PESICON_SINGLE_MODE::Single;
	return beginAcquire(SampleMode::exTrigger, _cycleTime, 1);
}

void PESiPanel::stopAcquire()
{
	Acquisition_Abort(hPESiAcqDesc);
}
bool PESiPanel::setFrames(int _frames)
{
	d_frames = _frames;
	d_frameSize = d_width * d_height * d_frames * sizeof(unsigned short);
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
bool PESiPanel::getPanelSize(int& _width, int& _height)
{
	_width = d_width;
	_height = d_height;
	return true;
}

bool PESiPanel::setPanelSize(int _width, int _height)
{
	d_width = _width;
	d_height = _height;
	return true;
}

bool PESiPanel::beginAcquire(SampleMode _sampleMode, int _cycleTime, int _frames)
{
	unsigned int iRet;
	unsigned long dwDemoParam = ACQ_SNAP;
	d_frameCount = 0;
	d_cycleTime = _cycleTime;
	setSampleMode(_sampleMode, _cycleTime);

	if (Acquisition_SetAcqData(hPESiAcqDesc, (void*)&dwDemoParam) != HIS_ALL_OK)
	{
		LOG_INFO("Acquisition_SetAcqData ʧ�ܣ�");
		return false;
	}

	setFrames(_frames);

	//����ɼ��ڴ�,���û�����
	if (d_frameSize != d_PESiDetBufferSize)
	{
		if (d_PESiDetAcqBuffer != nullptr)
			SAFE_DELETE(d_PESiDetAcqBuffer);

		d_PESiDetAcqBuffer = (unsigned char*)malloc(d_frameSize);
		d_PESiDetBufferSize = d_frameSize;
	}

	iRet = Acquisition_DefineDestBuffers(hPESiAcqDesc, (unsigned short*)d_PESiDetAcqBuffer, d_frames,
		d_height, d_width);

	if (iRet != HIS_ALL_OK)
	{
		LOG_ERROR("%sʧ�ܣ�������%d", "Acquisition_DefineDestBuffers", iRet);
		return false;
	}

	unsigned int bHisSeqFlag;

//	if (d_PESiContinusSingleMode == PESICON_SINGLE_MODE::Continus)
	bHisSeqFlag = HIS_SEQ_CONTINUOUS;
	//else if (d_PESiContinusSingleMode == PESICON_SINGLE_MODE::Single)
	//	bHisSeqFlag = HIS_SEQ_ONE_BUFFER;

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
