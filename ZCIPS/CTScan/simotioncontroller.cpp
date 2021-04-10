#include "stdafx.h"
#include "simotioncontroller.h"
#include "../Public/headers/CtrlData.h"
#include "../Public/util/thread.h"
#include <memory>
#include <thread>
#include <algorithm>
#include <functional>
#include <chrono>
#include <QTimer>

const unsigned short SimotionController::d_port = 8000;
const int SimotionController::d_packetSize = 256;
static in_addr hostAddr;

//TODO_DJ：改变发送命令循环的等待时间，取消tcpssever中的等待时间

SimotionController::SimotionController(std::map<Axis, AxisData>& _axisDataMap) 
	: ControllerInterface(_axisDataMap)
	, d_bytesReceived(0) , d_netWorkBuffer(new char[1000]), d_server
	(
		new TcpServer(sizeof(tagCOMM_PACKET1), 2, 4
			, [this](SOCKET _sock) { return initialSend(_sock); }
			, [this](char* _data, int _lenth) { pocessData(_data, _lenth); }
			, (hostAddr.S_un.S_addr = INADDR_ANY, hostAddr), d_port)
	)
{
	d_timer = new QTimer(this);
	d_timer->start(1000);
	connect(d_timer, &QTimer::timeout, this, &SimotionController::netCheckSlot);
}


SimotionController::~SimotionController()
{
	if(d_sendThread)
		d_sendThread->stopThread();

	delete[] d_netWorkBuffer;
}

bool SimotionController::getConnected()
{
	return d_connected;
}

bool SimotionController::checkReadyToScan()
{
	return d_ready && d_idle;
}

template<typename T1, typename T2>
inline bool GetResult(T1& t1, T2 t2, std::mutex& d_mutex, std::condition_variable& d_con)
{
	t1 = 0;
	std::unique_lock<std::mutex> lk(d_mutex);

	if (d_con.wait_for(lk, std::chrono::milliseconds(1000), [&] { return t1 == t2; }))
		return true;
		
	return false;
}

float SimotionController::readAxisPostion(Axis _axis)
{
	return d_axisPosition[_axis];
}

std::map<Axis, float> SimotionController::readAxisPostion()
{
	return d_axisPosition;
}

std::map<Axis, float> SimotionController::readAxisWorkZero()
{
	getAxisWorkZero();

	if (GetResult(d_receivedCmd, STS_WORKZERO, d_mutex, d_con))
		return d_axisWorkZero;

	return std::map<Axis, float>();
}

std::map<Axis, AxisCoordinateSwitchStatus> SimotionController::readAxisStatus()
{
	AxisCoordinateSwitchStatus sts;
	std::map<Axis, AxisCoordinateSwitchStatus> ret;

	for (auto& itr : d_axisDataMap)
	{
		if (itr.first == Axis::objRotation) 
		{
			sts.eln = 0;
			sts.elp = 0;
			sts.sdn = 0;
			sts.sdp = 0;
			sts.zeroFound = d_sysStatus.s.graduation_zero_found;
		}
		else if(itr.first == Axis::objTranslation)
		{
			sts.eln = d_sysStatus.s.translation_ELn;
			sts.elp = d_sysStatus.s.translation_ELp;
			sts.sdn = d_sysStatus.s.translation_SDn;
			sts.sdp = d_sysStatus.s.translation_SDp;
			sts.zeroFound = d_sysStatus.s.translation_zero_found;
		}
		else if (itr.first == Axis::rayLayer)
		{
			sts.eln = d_sysStatus.s.layer1_ELn;
			sts.elp = d_sysStatus.s.layer1_ELp;
			sts.sdn = d_sysStatus.s.layer1_SDn;
			sts.sdp = d_sysStatus.s.layer1_SDp;
			sts.zeroFound = d_sysStatus.s.layer1_zero_found;
		}
		else if (itr.first == Axis::detLayer)
		{
			sts.eln = d_sysStatus.s.layer2_ELn;
			sts.elp = d_sysStatus.s.layer2_ELp;
			sts.sdn = d_sysStatus.s.layer2_SDn;
			sts.sdp = d_sysStatus.s.layer2_SDp;
			sts.zeroFound = d_sysStatus.s.layer2_zero_found;
		}
		else if (itr.first == Axis::objRadial)
		{
			sts.eln = d_sysStatus.s.radial_ELn;
			sts.elp = d_sysStatus.s.radial_ELp;
			sts.sdn = d_sysStatus.s.radial_SDn;
			sts.sdp = d_sysStatus.s.radial_SDp;
			sts.zeroFound = d_sysStatus.s.radial_zero_found;
		}
		else if (itr.first == Axis::detRadial)
		{
			sts.eln = d_sysStatus.s.detRadial_ELn;
			sts.elp = d_sysStatus.s.radial_ELp;
			sts.sdn = d_sysStatus.s.radial_SDn;
			sts.sdp = d_sysStatus.s.radial_SDp;
			sts.zeroFound = d_sysStatus.s.radial_zero_found;
		}
		else if (itr.first == Axis::detTranslation)
		{
			sts.eln = d_sysStatus.s.detTranslation_ELn;
			sts.elp = d_sysStatus.s.detTranslation_ELp;
			sts.sdn = d_sysStatus.s.detTranslation_SDn;
			sts.sdp = d_sysStatus.s.detTranslation_SDp;
			sts.zeroFound = d_sysStatus.s.detTranslation_zero_found;
		}
		else if (itr.first == Axis::rayRadial)
		{
			sts.eln = d_sysStatus.s.rayRadial_ELn;
			sts.elp = d_sysStatus.s.rayRadial_ELp;
			sts.sdn = d_sysStatus.s.rayRadial_SDn;
			sts.sdp = d_sysStatus.s.rayRadial_SDp;
			sts.zeroFound = d_sysStatus.s.rayRadial_zero_found;
		}
		else if (itr.first == Axis::rayTranslation)
		{
			sts.eln = d_sysStatus.s.rayTranslation_ELn;
			sts.elp = d_sysStatus.s.rayTranslation_ELp;
			sts.sdn = d_sysStatus.s.rayTranslation_SDn;
			sts.sdp = d_sysStatus.s.rayTranslation_SDp;
			sts.zeroFound = d_sysStatus.s.rayTranslation_zero_found;
		}

		sts.coordinate = d_axisPosition[itr.first];
		ret[itr.first] = sts;
	}
	return ret;
}

bool SimotionController::readReadyStatus()
{
	return d_ready = d_sysStatus.s.ready;
}

bool SimotionController::readSaveStatus()
{
	return d_save = d_sysStatus.s.save;
}

bool SimotionController::clearSaveFlag()
{
	fillInCmdStructAndFillCmdList(CMD_CLEAR_SAVE_FLAG, nullptr, 0, false);
	return true;
}

bool SimotionController::readWaitNextScanStatus()
{
	return d_waitNextScan = d_sysStatus.s.waitNextScan;
}

bool SimotionController::readIdleStatus()
{
	return d_idle = d_sysStatus.s.idle;
}

std::map<Axis, float> SimotionController::readAxisSpeed()
{
	if (GetResult(d_receivedCmd, STS_AXIS_SPEED, d_mutex, d_con))
		return d_axisSpeed;

	return std::map<Axis, float>();
}

bool SimotionController::initialSend(SOCKET _sock)
{
	getSystemStatus();
	//getAxisPosition();
	//getControlfSystemStatus();
	d_sendThread.reset(new Thread([this](){ sendCmd(); }, d_deadThreadRun));
	d_sendThread->detach();
	return true;
}

bool SimotionController::stopAll()
{
	fillInCmdStructAndFillCmdList(CMD_STOP, nullptr, 0, false);
	return false;
}

bool SimotionController::initialiseController()
{
	fillInCmdStructAndFillCmdList(CMD_SEEK_ABS_ZERO, nullptr, 0, false);
	return false;
}

#define FILLX55XAAX5A	ptr->tagHead[0] = 0x55;\
						ptr->tagHead[1] = 0xaa;\
						ptr->tagHead[2] = 0x5a;

bool SimotionController::axisSeekZero(Axis _axis)
{
	char data[1];
	data[0] = char(_axis);
	fillInCmdStructAndFillCmdList(CMD_SEEK_AXIS_ABS_ZERO, data, 1, false);
	return true;
}

bool SimotionController::axisAbsMove(Axis _axis, float _pos)
{
	char data[5];
	data[0] = char(_axis);
	memcpy(data + 1, &_pos, sizeof(float));
	fillInCmdStructAndFillCmdList(CMD_AXIS_ABS_MOVE, data, 5, false);
	return true;
}

bool SimotionController::axisRelMove(Axis _axis, float _pos)
{
	char data[5];
	data[0] = char(_axis);
	memcpy(data + 1, &_pos, sizeof(float));
	fillInCmdStructAndFillCmdList(CMD_AXIS_REL_MOVE, data, 5, false);
	return true;
}

bool SimotionController::sliceAbsMove(float _pos)
{
	char data[4];
	memcpy(data, &_pos, sizeof(float));
	fillInCmdStructAndFillCmdList(CMD_SLICE_ABS_MOVE, data, 4, false);
	return true;
}

bool SimotionController::sliceRelMove(float _pos)
{
	char data[4];
	memcpy(data, &_pos, sizeof(float));
	fillInCmdStructAndFillCmdList(CMD_SLICE_REL_MOVE, data, 4, false);
	return true;
}

bool SimotionController::translationRelMove(float _pos)
{
	char data[4];
	memcpy(data, &_pos, sizeof(float));
	fillInCmdStructAndFillCmdList(CMD_RAYDET_TRANSLATION_REL_MOVE, data, 4, false);
	return true;
}

bool SimotionController::translationAbsMove(float _pos)
{
	char data[4];
	memcpy(data, &_pos, sizeof(float));
	fillInCmdStructAndFillCmdList(CMD_RAYDET_TRANSLATION_ABS_MOVE, data, 4, false);
	return true;
}

void SimotionController::getAixsValueAndNotify(std::map<Axis, float>& _value, char * _data, int _axisNum, int _typeCode)
{
	std::map<AxisPosEnum, float> temp; 

	for (int i = 0; i != _axisNum; ++i)
		temp[AxisPosEnum(i)] = *(float*)((_data + i * 4));
	
	for (auto& itr : temp)
	{
		for (auto& axisNamePosMapItr : AxisNamePosMap)
		{
			if (axisNamePosMapItr.second == itr.first)
				_value[axisNamePosMapItr.first] = itr.second;
		}
	}
	
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		d_receivedCmd = _typeCode;
		d_con.notify_all();
	}
}

void SimotionController::fillInCmdStructAndFillCmdList(int _cmd, char * _data, int _size, bool _consist)
{
	CommandType d_cmdData(sizeof(tagCOMM_PACKET) + _size + 1);   //一位校验和
	tagCOMM_PACKET* ptr = (tagCOMM_PACKET*)(d_cmdData.d_data);
	FILLX55XAAX5A;
	memcpy(d_cmdData.d_data + sizeof(tagCOMM_PACKET), _data, _size);
	ptr->typeCode = unsigned char(_cmd);
	ptr->tagLen = 3 + _size;
	unsigned char sum = 0;

	for (int i = 0; i < 3 + _size + 3; i++)
		sum += *(d_cmdData.d_data + i);

	*(d_cmdData.d_data + 3 + _size + 3) = sum;
	d_cmdList.pushBack(d_cmdData, _consist);
}

bool SimotionController::sendCmd()
{
	while (d_deadThreadRun)
	{
		CommandType cmd(0);

		if(d_cmdList.getNext(cmd))
			d_server->sendAsyn(cmd.d_data, cmd.d_size);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	return true;
}

void SimotionController::setConnectdSts()
{
	d_timeoutCount = 0;
}

//55  AA  5A    TP          BL           DW             VS
//  包头(3B)  分类码(1B) 包长(2B)    n字节参数字      校验和(1B)
void SimotionController::sendToControl(int _cmd, char * _data, int _size, bool _consist)
{
	fillInCmdStructAndFillCmdList(_cmd, _data, _size, _consist);
}

void SimotionController::pocessData(char* _package, int _size)
{
	setConnectdSts();
	int posecessedDataLenth = 0;
	memcpy(d_netWorkBuffer + d_bytesReceived, _package, _size);
	d_bytesReceived += _size;

	while (true)
	{
		struct tag
		{
			char tag1;
			char tag2;
			char tag3;
		};

		static tag temp{ char(0x55), char(0xAA), char(0x5A) };
		char* packetHead = d_netWorkBuffer + posecessedDataLenth;

		for (; packetHead != d_netWorkBuffer + d_bytesReceived - sizeof(tag); ++packetHead)
			if (memcmp(packetHead, &temp, sizeof(tag))== 0)
				break;

		if ((packetHead - d_netWorkBuffer) + sizeof(tagCOMM_PACKET1) < d_bytesReceived)
		{
			auto packetSize = *((unsigned short*)(d_netWorkBuffer + (packetHead - d_netWorkBuffer) + 4));

			if (d_bytesReceived - (packetHead - d_netWorkBuffer) < packetSize)
				break;
			else
			{
				decodePackages(packetHead, packetSize);
				posecessedDataLenth = packetSize + (packetHead - d_netWorkBuffer);
			}
		}
		else
			break;
	}

	if (posecessedDataLenth > 0)
	{
		memmove(d_netWorkBuffer, d_netWorkBuffer + posecessedDataLenth, d_bytesReceived - posecessedDataLenth);
		d_bytesReceived -= posecessedDataLenth;
	}
}

void SimotionController::reconnect()
{
	d_server->reAccept();
}

void SimotionController::netCheckSlot()
{
	if (++d_timeoutCount * 1000 > 5000)
	{
		if (d_connected == true)
		{
			d_connected = false;
			emit netWorkStsSginal(false);
		}
	}
	else
	{
		if (d_connected == false)
		{
			d_connected = true;
			emit netWorkStsSginal(true);
		}
	}
}

void SimotionController::decodePackages(char* _package, int _size)
{
	char typeCode = ((tagCOMM_PACKET1*)_package)->typeCode;
	int dataSize = ((tagCOMM_PACKET1*)_package)->tagLen;

	switch (typeCode) 
	{
	case	STS_SYSTEM_STATUS:
		d_sysStatus = *(SysStatus*)((_package + sizeof(tagCOMM_PACKET1)));
		break;
	case	STS_CONTROL_SYSTEM:
		d_ctrlSysSts = *(ControlSystemStatus*)((_package + sizeof(tagCOMM_PACKET1)));
		break;
	case	STS_GRADUATION_BASE:
		break;
	case	STS_WORKZERO:
	{
		int axisNum = dataSize / 4;
		//getAixsValueAndNotify(d_axisWorkZero, _package + sizeof(tagCOMM_PACKET1), axisNum, typeCode);
		break;
	}
	case	STS_ALL_COORDINATION:
	{
		int axisNum = dataSize / 4;
		getAixsValueAndNotify(d_axisPosition, _package + sizeof(tagCOMM_PACKET1), axisNum, typeCode);
		break;
	}
	case	STS_AXIS_SPEED:
	{
		int axisNum = dataSize / 4;
		//getAixsValueAndNotify(d_axisSpeed, _package + sizeof(tagCOMM_PACKET1), axisNum, typeCode);
		break;
	}
	case	STS_DIAGNICS_RESULT:
		break;
	case	STS_IO_REGISTER:
		break;
	case	STS_FAULT_CODE:
		break;
	case	STS_LASERMEAS_VALUE:
		break;
	default:
		break;
	}
}

void SimotionController::restartLineDet(int _detNum)
{
	//char data = char(_detNum);
	//fillInCmdStructAndFillCmdList(CMD_RESTART_DET, (char*)(&data), 1, false);
}

void SimotionController::switchRayDetCouple(int _couple)
{
	char data[2];
	data[0] = _couple;
	fillInCmdStructAndFillCmdList(CMD_DET_SELECT, data, 1, false);
}

void SimotionController::sendRayDetCoupleStatus(int _rayId, int _detId)
{
	d_rayId = _rayId;
	d_detId = _detId;
}

std::map<ControllerInterface::improtantStatus, bool>  SimotionController::readImportantStatus()
{
	d_importantStatus[network] = d_connected;
	d_importantStatus[seekingZero] = d_sysStatus.s.seekingZero;
	d_importantStatus[collide] = d_sysStatus.s.objVertical_org;
	d_importantStatus[changingRay] = d_sysStatus.s.changingRay;
	d_importantStatus[sliceThickAdjusting] = d_sysStatus.s.sliceThickAdjusting;
	d_importantStatus[changingDet] = d_sysStatus.s.changingDet;
	d_importantStatus[busy] = !d_sysStatus.s.idle;
	d_importantStatus[rayDetCoupleNotSame] = d_sysStatus.s.collimatorSelecting;
	return d_importantStatus;
}

bool SimotionController::startNextScan()
{
	fillInCmdStructAndFillCmdList(CMD_NEXT_SCAN, nullptr, 0, false);
	return true;
}

void SimotionController::getSystemStatus()
{
	char data[3];
	data[0] = char(STS_SYSTEM_STATUS);
	fillInCmdStructAndFillCmdList(CMD_POLL_STATUS, data, 3, true);
}

void SimotionController::getControlfSystemStatus()
{
	char data[1];
	data[0] = char(STS_CONTROL_SYSTEM);
	fillInCmdStructAndFillCmdList(CMD_POLL_STATUS, data, 1, true);
}

void SimotionController::getAxisPosition()
{
	char data[3];
	data[0] = char(STS_ALL_COORDINATION);
	data[1] = char(d_rayId);
	data[2] = char(d_detId);
	fillInCmdStructAndFillCmdList(CMD_POLL_STATUS, data, 3, false);
	return;
}

void SimotionController::getAxisSpeed()
{
	fillInCmdStructAndFillCmdList(CMD_UPLOAD_AXIS_SPEED, nullptr, 0, true);
}

void SimotionController::getAxisWorkZero()
{
	fillInCmdStructAndFillCmdList(CMD_UPLOAD_WORKZERO, nullptr, 0, true);
}

void SimotionController::stopGettingAxisPostion()
{
	;
}

void SimotionController::setAxisSpeed(std::map<Axis, float>& _speed)
{
	int size = _speed.size() * 5;
	char* data = new char[size];
	int i = 0;

	for (auto& pair : _speed)
	{
		data[i * 5] = char(pair.first);
		memcpy(data + i * 5 + 1, &pair.second, sizeof(float));
		++i;
	}
	
	fillInCmdStructAndFillCmdList(CMD_DOWNLOAD_AXIS_SPEED, data, size, false);
	delete[] data;
}

void SimotionController::setAxisWorkZero(std::map<Axis, float>& _workZero)
{
	int size = sizeof(FWorkZero);
	char* data = new char[size];
	int i = 0;

	for (auto& pair : _workZero)
		memcpy(data + int(pair.first) * 4, &pair.second, sizeof(float));

	fillInCmdStructAndFillCmdList(CMD_DOWNLOAD_AXIS_SPEED, data, size, false);
	delete[] data;
}
