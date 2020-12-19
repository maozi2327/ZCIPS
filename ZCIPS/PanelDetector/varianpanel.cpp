#include "stdafx.h"
#include <Windows.h>
#include "varianpanel.h"

VarianPanel::VarianPanel()
	: Panel()
	, d_fixedThreadPool(2)
{
	d_fixedThreadPool.excute([=]() { netWorkThread(); });
}

VarianPanel::~VarianPanel()
{
}

bool VarianPanel::connectPanel()
{
	SOpenReceptorLink  orl;
	memset(&orl, 0, sizeof(SOpenReceptorLink));
	orl.StructSize = sizeof(SOpenReceptorLink);
	auto dllPath = QCoreApplication::applicationDirPath();
	strcpy_s(orl.RecDirPath, dllPath.toLocal8Bit());
	int result = vip_open_receptor_link(&orl);

	if (result != HCP_NO_ERR)
		return false;

	SCheckLink clk;
	memset(&clk, 0, sizeof(SCheckLink));
	clk.StructSize = sizeof(SCheckLink);
	result = vip_check_link(&clk);
	int i = 0;

	while (result != HCP_NO_ERR && i++ < 5)
	{
		Sleep(1000);
		result = vip_check_link(&clk);
	}

	if (result != HCP_NO_ERR)
	{
		int ignore = 0;
		LOG_ERROR("vip_check_link() returned %d", result);
		return false;
	}

	result = HCP_NO_ERR;
	SSysInfo SystemInfo;
	memset(&SystemInfo, 0, sizeof(SystemInfo));
	SystemInfo.StructSize = sizeof(SSysInfo);
	result = vip_get_sys_info(&SystemInfo);
	d_width = SystemInfo.MxColsPerFrame;
	d_height = SystemInfo.MxLinesPerFrame;
	d_frameSize = d_width * d_height * sizeof(unsigned short);
	SCorrections correctSet;
	memset(&correctSet, 0, sizeof(SCorrections));
	correctSet.StructSize = sizeof(SCorrections);
	correctSet.Gain = FALSE;
	correctSet.Ofst = FALSE;
	correctSet.Dfct = FALSE;
	vip_set_correction_settings(&correctSet);
	return false;
}

bool VarianPanel::setCycleTime(int _milliseconds)
{
	auto itr = d_modeBinPfVgFps.find(d_mode);

	if (itr == d_modeBinPfVgFps.end())
		return false;

	auto fps = itr->second.fps;
	auto frameRate = double(1000) / _milliseconds;

	if (frameRate > fps)
		return false;

	auto result = vip_set_frame_rate(d_mode, frameRate);

	if (result != VIP_NO_ERR)
	{
		LOG_ERROR("vip_set_frame_rate() returned %d", result);
		return false;
	}

	return true;
}

bool VarianPanel::setFrames(int _frames)
{
	d_frames = _frames;
	return true;
}

bool VarianPanel::setSampleMode(SampleMode _sampleMode, int _cycleTime)
{
	int result;

	switch (_sampleMode)
	{
	case softTrigger:
		result = vip_set_user_sync(d_mode, FALSE);
		break;
	case exTrigger:
		result = vip_set_user_sync(d_mode, TRUE);
		break;
	default:
		break;
	}

	if (result != VIP_NO_ERR)
	{
		LOG_ERROR("vip_set_user_sync() returned %d", result);
		return false;
	}

	return setCycleTime(_cycleTime);
}

bool VarianPanel::setVarianMode(int _mode)
{
	if (d_modeBinPfVgFps.find(_mode) != d_modeBinPfVgFps.end())
		return false;

	auto result = vip_select_mode(_mode);

	if (result != VIP_NO_ERR)
	{
		LOG_ERROR("vip_select_mode() returned %d", result);
		return false;
	}

	return true;
}

bool VarianPanel::setGainFactor(int _gainFactor)
{
	return false;
}

bool VarianPanel::getConnected()
{
	return d_netWork;
}

bool VarianPanel::getReady()
{
	return false;
}

bool VarianPanel::beginSoftwareTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _frames, 
	int _cycleTime, int _gainFactor)
{
	setFrameCallback(_imageProcessCallBack);
	return beginAcquire(SampleMode::softTrigger, _cycleTime, _frames);
}

bool VarianPanel::beginExTriggerAcquire(std::function<void(unsigned short*)> _imageProcessCallBack, int _cycleTime, int _gainFactor)
{
	setFrameCallback(_imageProcessCallBack);
	return beginAcquire(SampleMode::exTrigger, _cycleTime, 1);
}

void VarianPanel::stopAcquire()
{
	int result = vip_fluoro_record_stop();

	if (result != VIP_NO_ERR)
		LOG_ERROR("vip_fluoro_record_stop() returned %d", result);

	result = vip_fluoro_grabber_stop();

	if (result != VIP_NO_ERR)
		LOG_ERROR("vip_fluoro_grabber_stop() returned %d", result);

	vip_reset_state();

	if (result != VIP_NO_ERR)
		LOG_ERROR("vip_fluoro_grabber_stop() returned %d", result);
}

void VarianPanel::setFrameCallback(std::function<void(unsigned short*)> _imageProcessCallBack)
{
	d_imageProcCallback = _imageProcessCallBack;
}

int VarianPanel::caculateExTriggerSampleTime(int _cycleTime)
{
	return _cycleTime + d_exTriggerTimeMargin;
}

QWidget * VarianPanel::getWidget()
{
	return d_widget;
}

bool VarianPanel::beginAcquire(SampleMode _sampleMode, int _cycleTime, int _frames)
{
	int result = VIP_NO_ERR;
	int frameRate = int(double(1000) / _cycleTime);
	SModeInfo modeInfo;
	vip_get_mode_info(d_mode, &modeInfo);

	if (modeInfo.FrameRate < frameRate)
	{
		LOG_ERROR("帧率设置过高");
		return false;
	}

	SSeqPrms seqPrms;
	memset(&seqPrms, 0, sizeof(SSeqPrms));
	
	if (_sampleMode == SampleMode::softTrigger)
		seqPrms.StopAfterN = _frames;
	else if(_sampleMode == SampleMode::exTrigger)
		seqPrms.StopAfterN = 0;

	seqPrms.StructSize = sizeof(SSeqPrms);
	seqPrms.NumBuffers = _frames;
	result = vip_fluoro_set_prms(HCP_FLU_SEQ_PRMS, &seqPrms);

	if (result != VIP_NO_ERR)
	{
		LOG_ERROR("vip_fluoro_set_prms() returned %d", result);
		return false;
	}

	_frames = seqPrms.NumBuffers;
	SCorrections corr;
	memset(&corr, 0, sizeof(SCorrections));
	corr.StructSize = sizeof(SCorrections);
	result = vip_get_correction_settings(&corr);
	SAcqPrms acqPrms;
	memset(&acqPrms, 0, sizeof(SAcqPrms));
	acqPrms.StructSize = sizeof(SAcqPrms);
	acqPrms.StartUp = HCP_REC_IDLE;
	acqPrms.CorrType = HCP_CORR_NONE;
	acqPrms.CorrFuncPtr = NULL; 
	acqPrms.ReqType = 0;
	result = vip_fluoro_grabber_start(&acqPrms);

	if (result != VIP_NO_ERR)
	{
		LOG_ERROR("vip_fluoro_grabber_start() returned %d", result);
		int res2 = vip_reset_state();

		if (res2 != VIP_NO_ERR)
		{
			LOG_ERROR("\vip_reset_state() returned %d", res2);
		}

		return false;
	}

	d_liveParams = (SLivePrms*)acqPrms.LivePrmsPtr;
	d_grabbingIsActive = true;
	result = vip_fluoro_get_event_name(HCP_FG_FRM_TO_DISP, d_frameReadyName);

	d_fixedThreadPool.excute([=]() { imageProcessThread(); });
	result = vip_fluoro_record_start();

	if (result != VIP_NO_ERR)
	{
		LOG_ERROR("vip_fluoro_record_start() returned %d", result);
		d_grabbingIsActive = false;
		vip_fluoro_grabber_stop();
		vip_reset_state();
		return false;
	}

	return true;
}

//void UnicodeToByte(CString& str, char *&ch)
//{
//	int len = WideCharToMultiByte(CP_ACP, 0, str, int(wcslen(str)), NULL, 0, NULL, NULL);  //获取字节数  
//	ch = new char[len + 1];	                                       //定义字符串数组，由于要添加\r，所以要加2
//	WideCharToMultiByte(CP_ACP, 0, str, int(wcslen(str)), ch, len, NULL, NULL); //复制字符串  
//	ch[len] = '\r';
//}
//单字节转多字节函数

void ByteToUnicode(char* ch, int lenth, WCHAR *&str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, ch, lenth, 0, NULL);
	str = new WCHAR[lenth + 1];
	MultiByteToWideChar(CP_ACP, 0, ch, lenth, str, len);
	*(str + len) = TEXT('\r');
}

void VarianPanel::imageProcessThread()
{
	d_idle = true;

	if (d_liveParams == nullptr )
	{
		LOG_ERROR("zero LiveParam");
		d_idle = false;
		return;
	}

	const int timeoutval = 500;
	WCHAR* wName;
	ByteToUnicode(d_frameReadyName, strlen(d_frameReadyName), wName);
	HANDLE	hFrameEvent = CreateEvent(NULL, FALSE, FALSE, wName);

	if (hFrameEvent == NULL) 
	{
		LOG_ERROR("hFrameEvent is NULL");
		d_idle = false;
		return;
	}

	while (d_grabbingIsActive && d_liveParams)
	{
		if (WaitForSingleObject(hFrameEvent, timeoutval) != WAIT_TIMEOUT)
		{
			if (d_liveParams == NULL)
			{
				LOG_ERROR("d_liveParams is NULL");
				d_idle = false;
				return;
			}

			WORD* imPtr = (WORD*)d_liveParams->BufPtr;
			d_copyBuffer = (unsigned char*)malloc(d_frameSize);
			memcpy((char*)(d_copyBuffer), imPtr, d_frameSize);
			d_imageProcCallback((unsigned short*)d_copyBuffer);

			if (d_liveParams->NumFrames == d_frames)
				break;
		}
	}

	if (hFrameEvent)
		CloseHandle(hFrameEvent);

	stopAcquire();
	d_idle = false;
}

void VarianPanel::netWorkThread()
{
	connectPanel();

	if (!d_idle)
	{
		SCheckLink clk;
		memset(&clk, 0, sizeof(SCheckLink));
		clk.StructSize = sizeof(SCheckLink);
		auto result = vip_check_link(&clk);

		if (result != VIP_NO_ERR)
		{
			d_netWork = false;
			connectPanel();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}

int VarianPanel::getSampleTime()
{
	return d_sampleTime;
}

std::pair<int, int> VarianPanel::getPanelSize()
{
	return std::pair<int, int>(d_width, d_height);
}

float VarianPanel::getPixelSize()
{
	return d_pixelSize;
}

bool VarianPanel::setPanelSize(int _width, int _height)
{
	return false;
}

size_t VarianPanel::getFrameSize()
{
	return size_t();
}
