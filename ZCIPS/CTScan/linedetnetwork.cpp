#include "stdafx.h"
#include "LineDetNetWork.h"
#include "../Public/util/tcpserver.h"
#include "../Public/util/thread.h"
#include <chrono>
#include <algorithm>
#include <vector>

static in_addr hostAddr;

LineDetNetWork::LineDetNetWork(unsigned short _port, unsigned short _fifoMask, unsigned short _channelDepth, unsigned short _delayTime,
	unsigned short _intTime, unsigned short _ampSize, std::vector<unsigned int> _blockModuleVec, int _detNum)
	: d_server(
		new TcpServer(4, 4, 0
		, [this](SOCKET _sock){ return setParameterAfterConnect(_sock); }
		, [this](char* _char, int _size) { pocessData(_char, _size); }
		, (hostAddr.S_un.S_addr = INADDR_ANY, hostAddr), 4000)
	)
	, d_fifoMask(_fifoMask), d_channelDepth(_channelDepth)
	, d_delayTime(_delayTime), d_intTime(_intTime), d_ampSize(_ampSize)
	, d_netWorkBuffer(new char[2u << 12]), d_bytesReceived(0), d_blockModuleMap(_blockModuleVec)
	, d_isScanning(false), d_connected(false)
	, d_detNum(_detNum)
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

	if (!ARMTest()) return false;
	if (!ChannelSelect()) return false;
	if (!ChannelDepthSet()) return false;
	if (!StartCI()) return false;
	if (!DetectorTest()) return false;
	if (!SetDelayTime(d_delayTime)) return false;
	if (!SetIntTime(d_intTime)) return false;
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
	//ʵ��FIFO����ȣ�Ҳ�ǵ�����������ARM��FIFO�Ĵ���(16λ����λ��67���������ݣ�������134�ζ�����)
	//�˴�channel_depth+3�������а����ֶȺš�������ź�У���3������	
	CMD_STRUCT cmdInfo{ 16, CMD_CHANNEL_DEPTH_SET, (unsigned int)((d_channelDepth + 3) * 2), 0 };

	if (d_server->sendSyn((char*)(&cmdInfo), sizeof(cmdInfo)) == sizeof(cmdInfo))
		return true;

	return false;
}

bool LineDetNetWork::StartCI()
{
	CMD_STRUCT cmdInfo{ 16, CMD_START_CI, 0, 0 };

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

bool LineDetNetWork::SetIntTime(int _intTime)
{
	CMD_STRUCT cmdInfo{ 16, CMD_SET_INT_TIME, (unsigned int)_intTime, 0 };
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

bool LineDetNetWork::ReadIntTime()
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

bool LineDetNetWork::startExtTrigAcquire()
{
	d_dataList.clear();
	d_recvType = DATA;
	d_isScanning = false;
	CMD_STRUCT cmdInfo{ 16, CMD_INTERNAL_COLLECT, 1, 0 };

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
	return 0;
}

int LineDetNetWork::CollectUsefulData(char * _buff, int _size)
{
	int pulseNum = (_size - sizeof(int)) / d_dataSizePerPulse; //ǰ���ĸ��ֽ������ݳ���
	int smallBS = (d_channelDepth + 3) * sizeof(unsigned int); //�ֶȺ�+�����+����+У���
	int smallBD = d_channelDepth * sizeof(unsigned int);
	for (int pulseIndex = 0; pulseIndex != pulseNum; ++pulseIndex)
	{
		memmove(_buff, _buff + sizeof(unsigned int), 2 * sizeof(unsigned int));
		//�ֶȼ���+�������+��ͨ����
		unsigned long* item = new unsigned long[d_channelNum + 2];
		int smIndex = 0;

		for (int smIndex = 0; smIndex != d_smallBoardNum; ++smIndex)
		{
			memmove(_buff + 2 * sizeof(unsigned int) + smIndex * smallBD	//�����һ�����������ݳ���
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
	d_dataList.clear();
}
