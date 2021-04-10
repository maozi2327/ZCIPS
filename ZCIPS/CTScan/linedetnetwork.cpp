#include "stdafx.h"
#include "LineDetNetWork.h"
#include "../Public/util/tcpserver.h"
#include "../Public/util/thread.h"
#include <chrono>
#include <algorithm>
#include <vector>

static in_addr hostAddr;

LineDetNetWork::LineDetNetWork(unsigned short _port, unsigned long _fifoMask, unsigned short _channelDepth, unsigned short _delayTime,
	unsigned short _sampleTime, unsigned short _ampSize, std::vector<unsigned int> _blockModuleVec, int _detNum)
	: d_fifoMask(_fifoMask), d_channelDepth(_channelDepth)
	, d_delayTime(_delayTime), d_sampleTime(_sampleTime), d_ampSize(_ampSize)
	, d_netWorkBuffer(new char[2u << 12]), d_bytesReceived(0), d_blockModuleMap(_blockModuleVec)
	, d_isScanning(false), d_connected(false)
	, d_detNum(_detNum)
{
	caculateChannel(); 
	d_server.reset(
		new TcpServer(4, 4, 0, [this](SOCKET _sock) { return setParameterAfterConnect(_sock); },
			[this](char* _char, int _size) { pocessData(_char, _size); }, (hostAddr.S_un.S_addr = INADDR_ANY, hostAddr), 4000));
	//d_netWorkCheckThread.reset
	//(
	//	new Thread
	//	(
	//		[=]()
	//		{
	//			std::this_thread::sleep_for(std::chrono::seconds(20));

	//			while (d_deadThreadRunFlag)
	//			{
	//				if (!d_isScanning && !StartCI())
	//				{
	//					d_connected = false;
	//					d_server->reAccept();
	//					emit netWorkStatusSignal(d_detNum, d_connected);
	//				}

	//				int counter = 0;

	//				while(d_deadThreadRunFlag && counter++ != 10)
	//					std::this_thread::sleep_for(std::chrono::seconds(1));
	//			}
	//		}
	//		, d_deadThreadRunFlag
	//	)
	//);	
	//d_netWorkCheckThread->detach();
}

LineDetNetWork::~LineDetNetWork()
{
	d_deadThreadRunFlag;
	delete[] d_netWorkBuffer;
}

bool LineDetNetWork::getConnected()
{
	return d_connected;
}

bool LineDetNetWork::setParameterAfterConnect(SOCKET _sock)
{
	int val = 0;
	
	if(setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, (char*)(&val), sizeof(int)) == SOCKET_ERROR)
		return false;

	if(setsockopt(_sock, SOL_SOCKET, SO_RCVBUF, (char*)(&val), sizeof(int)) == SOCKET_ERROR)
		return false;

	if(setsockopt(_sock, SOL_SOCKET, SO_SNDBUF, (char*)(&val), sizeof(int)) == SOCKET_ERROR)
		return false;

	//TODO_DJ:启动的时候挂了
	if (!ARMTest()) return false;
	if (!ChannelSelect()) return false;
	if (!ChannelDepthSet()) return false;
	if (!StartCI()) return false;
	if (!DetectorTest()) return false;
	if (!SetDelayTime(d_delayTime)) return false;
	if (!SetSampleTime(d_sampleTime)) return false;
	if (!SetAmpSize(d_ampSize)) return false;

	d_connected = true;
	return true;
}

void LineDetNetWork::pocessData(char * _package, int _size)
{
	int posecessedDataLenth = 0;
	memcpy(d_netWorkBuffer + d_bytesReceived, _package, _size);
	d_bytesReceived += _size;

	while (true)
	{
		if ((d_bytesReceived - posecessedDataLenth) < sizeof(int))
			break;

		int packetSize = *(unsigned int*)(d_netWorkBuffer + posecessedDataLenth);

		if (d_bytesReceived - posecessedDataLenth < packetSize)
			break;
		else
		{
			DecodePackages(d_netWorkBuffer + posecessedDataLenth, packetSize);
			posecessedDataLenth += packetSize;
		}
	}

	if (posecessedDataLenth > 0)
	{
		memmove(d_netWorkBuffer, d_netWorkBuffer + posecessedDataLenth, d_bytesReceived - posecessedDataLenth);
		d_bytesReceived -= posecessedDataLenth;
	}
}

bool LineDetNetWork::recvServer_DATA()
{
	return 0;
}

bool LineDetNetWork::NetCheck()
{
	return 0;
}

bool LineDetNetWork::NetSetup()
{
	return 0;
}

template<typename T1, typename T2>
inline bool GetResult(T1& t1, T2 t2, std::condition_variable& _con, std::mutex& _mutex)
{
	std::unique_lock<std::mutex> lk(_mutex);

	if (_con.wait_for(lk, std::chrono::milliseconds(1000), [&t1, &t2] { return t1 == t2; }))
		return true;
	else
		return false;
}

bool LineDetNetWork::ARMTest()
{
	CMD_STRUCT cmdInfo{ 16, CMD_ARM_TEST , 0xffff, 1 };
	d_recvType = CMD;
	d_dataList.clear();

	if(d_server->sendSyn((char*)&cmdInfo, sizeof(CMD_STRUCT)) == sizeof(cmdInfo))
		return GetResult(d_cmdType, CMD_ARM_TEST, d_con, d_mutex);

	return false;
}

bool LineDetNetWork::ChannelSelect()
{
	CMD_STRUCT cmdInfo{ 16, CMD_CHANNEL_SELECT, d_fifoMask, 0 };
	
	if (d_server->sendSyn((char*)(&cmdInfo), sizeof(cmdInfo)) == sizeof(cmdInfo))
		return true;

	return false;
}

bool LineDetNetWork::ChannelDepthSet()
{
	//实际FIFO的深度，也是单采样脉冲下ARM读FIFO的次数(16位总线位宽、67个整型数据，将进行134次读操作)
	//此处channel_depth+3是数据中包含分度号、脉冲序号和校验和3个数据	
	CMD_STRUCT cmdInfo{ 16, CMD_CHANNEL_DEPTH_SET, (unsigned int)((d_channelDepth + 3) * 2), 0 };

	if (d_server->sendSyn((char*)(&cmdInfo), sizeof(cmdInfo)) == sizeof(cmdInfo))
		return true;

	return false;
}

bool LineDetNetWork::caculateChannel()
{
	auto tempMask = d_fifoMask;
	d_smallBoardNum = 0;

	while (tempMask)
	{
		tempMask &= (tempMask - 1);
		++d_smallBoardNum;
	}

	d_channelNum = (d_smallBoardNum * 8 - d_blockModuleMap.size()) * 8;
	d_dataSizePerPulse = sizeof(int) * (d_smallBoardNum * (d_channelDepth + 3));
	return true;
}

bool LineDetNetWork::StartCI()
{
	CMD_STRUCT cmdInfo{ 16, CMD_START_CI, 0, 0 };
	d_graduationCount = 0;

	if (d_server->sendSyn((char*)(&cmdInfo), sizeof(cmdInfo)) == sizeof(cmdInfo))
		return true;

	return false;
}

bool LineDetNetWork::FPGATest()
{
	return 0;
}

bool LineDetNetWork::DetectorTest()
{
	CMD_STRUCT cmdInfo{ 16, CMD_DETECTOR_TEST, d_channelDepth, 1 };
	d_recvType = PARAMETER;
	d_dataList.clear();
	
	if(d_server->sendSyn((char*)&cmdInfo, sizeof(cmdInfo)) == sizeof(cmdInfo))
		return GetResult(d_returnSize, d_dataSizePerPulse, d_con, d_mutex);

	return false;
}

bool LineDetNetWork::SetAmpSize(int _ampSize)
{
	CMD_STRUCT cmdInfo{ 16, CMD_SET_AMP_SIZE, (unsigned int)_ampSize, 1 };
	d_recvType = PARAMETER;
	d_dataList.clear();

	if(d_server->sendSyn((char*)&cmdInfo, sizeof(cmdInfo)) == sizeof(cmdInfo))
		return GetResult(d_returnSize, d_dataSizePerPulse, d_con, d_mutex);

	return false;
}

bool LineDetNetWork::SetSampleTime(int _sampleTime)
{
	CMD_STRUCT cmdInfo{ 16, CMD_SET_INT_TIME, (unsigned int)_sampleTime, 0 };
	d_recvType = PARAMETER;
	d_dataList.clear();

	if(d_server->sendSyn((char*)&cmdInfo, sizeof(cmdInfo)) == sizeof(cmdInfo))
		return GetResult(d_returnSize, d_dataSizePerPulse, d_con, d_mutex);

	return false;
}

bool LineDetNetWork::SetDelayTime(int _delayTime)
{
	CMD_STRUCT cmdInfo{ 16, CMD_SET_DELAY_TIME, (unsigned int)_delayTime, 1 };
	d_recvType = PARAMETER;
	d_dataList.clear();
	
	if(d_server->sendSyn((char*)&cmdInfo, sizeof(cmdInfo)) == sizeof(cmdInfo))
		return GetResult(d_returnSize, d_dataSizePerPulse, d_con, d_mutex);

	return false;
}

bool LineDetNetWork::ReadAmpSize()
{
	CMD_STRUCT cmdInfo{ 16, CMD_READ_AMP_SIZE, 0x0, 1 };
	d_recvType = PARAMETER;
	d_dataList.clear();

	if(d_server->sendSyn((char*)&cmdInfo, sizeof(cmdInfo)) == sizeof(cmdInfo))
		return GetResult(d_returnSize, d_dataSizePerPulse, d_con, d_mutex);

	return false;
}

bool LineDetNetWork::ReadSampleTime()
{
	CMD_STRUCT cmdInfo{ 16, CMD_READ_INT_TIME, 0x0, 1 };
	d_recvType = PARAMETER;
	d_dataList.clear();

	if(d_server->sendSyn((char*)&cmdInfo, sizeof(cmdInfo)) == sizeof(cmdInfo))
		return GetResult(d_returnSize, d_dataSizePerPulse, d_con, d_mutex);

	return false;
}

bool LineDetNetWork::ReadDelayTime()
{
	CMD_STRUCT cmdInfo{ 16, CMD_READ_DELAY_TIME, 0x0, 1 };
	d_recvType = PARAMETER;
	d_dataList.clear();

	if(d_server->sendSyn((char*)&cmdInfo, sizeof(cmdInfo)) == sizeof(cmdInfo))
		return GetResult(d_returnSize, d_dataSizePerPulse, d_con, d_mutex);

	return false;
}

bool LineDetNetWork::setChannelMask(int _mask)
{
	d_fifoMask = _mask;
	d_blockModuleMap.resize(0);
	caculateChannel();
	ChannelSelect();
	return false;
}

bool LineDetNetWork::setChannelDepth(int _depth)
{
	return false;
}

bool LineDetNetWork::startExtTrigAcquire()
{
	d_dataList.clear();
	d_recvType = DATA;
	d_isScanning = false;
	d_graduationCount = 0;
	d_lastGraduation = -1;
	CMD_STRUCT cmdInfo{ 16, CMD_INTERNAL_COLLECT, 1, 0 };

	if (d_server->sendSyn((char*)(&cmdInfo), sizeof(cmdInfo)) == sizeof(cmdInfo))
		return true;

	return false;
}

bool LineDetNetWork::startInternalTrigAcquire(int _mode, unsigned long* _data)
{
	d_dataList.clear();
	d_recvType = INTERNAL_COLLECT;
	d_internalCollectTempData = _data;
	d_isScanning = false;
	d_graduationCount = 0;
	CMD_STRUCT cmdInfo{ 16, CMD_INTERNAL_COLLECT, _mode, 0 };

	if (d_server->sendSyn((char*)(&cmdInfo), sizeof(cmdInfo)) == sizeof(cmdInfo))
		return true;

	return false;
}

bool LineDetNetWork::stopInternalTrigAcquire()
{
	d_isScanning = false;
	CMD_STRUCT cmdInfo{ 16, CMD_INTERNAL_COLLECT, 0, 0 };

	if (d_server->sendSyn((char*)(&cmdInfo), sizeof(cmdInfo)) == sizeof(cmdInfo))
		return true;

	return false;
}

void LineDetNetWork::stopAcquire(bool _finished)
{
	d_isScanning = _finished;
	clearRowList();
}

void LineDetNetWork::DecodePackages(char * _buff, int _size)
{
	CMD_STRUCT cmdFeedback;

	switch (d_recvType)
	{
	case CMD:
		if(_size == sizeof(CMD_STRUCT))
		{
			std::lock_guard<std::mutex> lk(d_mutex);
			d_cmdType = ((CMD_STRUCT*)_buff)->cmd;
			d_con.notify_one();
		}
		break;
	case PARAMETER:
	{
		std::lock_guard<std::mutex> lk(d_mutex);
		d_returnSize = CollectUsefulData(_buff, _size);
		d_con.notify_one();
	}
		break;
	case DATA:
	case INTERNAL_COLLECT:
		CollectUsefulData(_buff, _size);
		break;		
	}
}

int LineDetNetWork::getListItemSize()
{
	return (d_channelNum + 2);
}

int LineDetNetWork::getListItemNum()
{
	return d_dataList.getListSize();
}

int LineDetNetWork::getGraduationCount()
{
	return d_graduationCount;
}

int LineDetNetWork::CollectUsefulData(char * _buff, int _size)
{
	int pulseNum = (_size - sizeof(int)) / d_dataSizePerPulse; //前面四个字节是数据长度
	int smallBS = (d_channelDepth + 3) * sizeof(unsigned int); //分度号+脉冲号+数据+校验和
	int smallBD = d_channelDepth * sizeof(unsigned int);

	for (int pulseIndex = 0; pulseIndex != pulseNum; ++pulseIndex)
	{
		memmove(_buff, _buff + sizeof(unsigned int), 2 * sizeof(unsigned int));
		//分度计数+脉冲计数+总通道数
		unsigned long* item = new unsigned long[d_channelNum + 2];
		int smIndex = 0;

		for (int smIndex = 0; smIndex != d_smallBoardNum; ++smIndex)
		{
			memmove(_buff + 2 * sizeof(unsigned int) + smIndex * smallBD	//下面第一个数据是数据长度
				, _buff + sizeof(unsigned int) + pulseIndex * d_dataSizePerPulse + smIndex * smallBS + 2 * sizeof(unsigned int)
				, smallBD);
		}

		int nonBlockDataIndex = 0;
		memcpy(item, _buff, 2 * sizeof(unsigned int));
		unsigned long* dataHead = item + 2;
		auto moduleDataSize = 8 * sizeof(unsigned int);
		auto virtualModuleNum = d_smallBoardNum * 8;
		int before = -1, end = virtualModuleNum, dataCopied = 0;

		for (auto& blockIndex : d_blockModuleMap)
		{
			auto copyModuleNum = blockIndex - before - 1;
			memcpy((char*)(dataHead) + dataCopied
				, _buff + 2 * sizeof(unsigned int) + (before + 1) * moduleDataSize
				, copyModuleNum * moduleDataSize);
			dataCopied += copyModuleNum * moduleDataSize;
			before = blockIndex;
		}
		
		memcpy((char*)(dataHead)+dataCopied
			, _buff + 2 * sizeof(unsigned int) + (before + 1) * moduleDataSize
			, (end - before -1) * moduleDataSize);

		if(d_recvType == INTERNAL_COLLECT)
			memcpy(d_internalCollectTempData, dataHead + 2, d_channelNum * sizeof(unsigned long));

		if (*item != d_lastGraduation)
		{
			++d_graduationCount;
			d_lastGraduation = *item;
		}

		d_dataList.push_back(item);
	}

	return _size - sizeof(int);
}

LineDetList * LineDetNetWork::getRowList()
{
	return d_dataList.getList();
}

void LineDetNetWork::clearRowList()
{
	d_graduationCount = 0;
	d_dataList.clear();
}

int LineDetNetWork::getChannelNum()
{
	return d_channelNum;
}
