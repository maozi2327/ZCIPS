#pragma once
#include "controllerinterface.h"
#include "../Public/util/tcpserver.h"
#include <list>
#include <memory>
#include <condition_variable>
#include <mutex>
#include "../Public/headers/ctrldata.h"
#include "../Public/headers/list.h"
#include <atomic>

struct CommandType
{
	int d_cmd;
	char* d_data;
	int d_size;
	CommandType(int _size)
	{
		d_data = new char[_size];
		d_size = _size;
	}

	void operator=(const CommandType& _command)
	{
		char* temp = nullptr;
		
		if (_command.d_size != 0)
		{
			temp = new char[_command.d_size];
			memcpy(temp, _command.d_data, _command.d_size);
		}

		if (d_data != nullptr)
			delete[] d_data;

		d_data = temp;
		d_size = _command.d_size;
		d_cmd = _command.d_cmd;
	}

	CommandType(const char* _data, int _size)
	{
		d_data = new char[_size];
		d_size = _size;
		memcpy(d_data, _data, _size);
	}
	CommandType(const CommandType& _commandType)
	{
		d_data = new char[_commandType.d_size];
		d_size = _commandType.d_size;
		memcpy(d_data, _commandType.d_data, d_size);
	}

	CommandType(CommandType&& _commandType)
	{
		d_data = _commandType.d_data;
		d_size = _commandType.d_size;
	}
	~CommandType()
	{
		if (d_data != nullptr)
		{
			delete[] d_data;
			d_size = 0;
			d_data = nullptr;
		}
	}
	friend bool operator==(const CommandType& _commandLeft, const CommandType& _commandRight)
	{
		return _commandLeft.d_cmd == _commandRight.d_cmd;
	}
};

struct tagCOMM_PACKET 
{
	unsigned char tagHead[3];						//包头(0x55,0xaa,0x5a)
	unsigned char typeCode;							//类型码
	unsigned short tagLen;							//包长(=参数字节数+3, 实际命令数据包长度=包长+4)
};

struct tagCOMM_PACKET1 
{
	unsigned char tagHead[3];						//包头(0x55,0xaa,0x5a)
	unsigned char typeCode;							//类型码
	unsigned short tagLen;							//包长(=参数字节数+3, 实际命令数据包长度=包长+4)
};

class Thread;
class SimotionController : public ControllerInterface
{
private:
	std::unique_ptr<TcpServer> d_server;
	const static unsigned short d_port;
	const static int d_packetSize;
	
	SysStatus d_sysStatus;
	ControlSystemStatus d_ctrlSysSts;									//控制系统状态字
	List<CommandType> d_cmdList;
	std::condition_variable d_con;
	std::mutex d_mutex;
	int d_receivedCmd;
			
	bool d_save;
	bool d_ready;
	bool d_waitNextScan;
	bool d_idle;
	std::atomic<bool> d_deadThreadRun;
	std::unique_ptr<Thread> d_sendThread;
	char* d_netWorkBuffer;
	int d_bytesReceived;
	void getAixsValueAndNotify(std::map<Axis, float>& _value, char* _data, int _axisNum, int _typeCode);
	void fillInCmdStructAndFillCmdList(int _cmd, char* _data, int _size, bool _consist);
	int d_rayId;
	int d_detId;
protected:
	virtual bool sendCmd();
	virtual void setConnectdSts(); 
	void pocessData(char* _package, int _size);
	void reconnect();
private slots:
	void netCheckSlot();
public:
	SimotionController(std::map<Axis, AxisData>& _axisDataMap);
	~SimotionController();
	
	virtual bool getConnected();
	virtual bool checkReadyToScan();

	virtual bool initialSend(SOCKET _sock);
	virtual bool stopAll();
	virtual bool initialiseController();
	virtual bool axisSeekZero(Axis _axis);
	virtual bool axisAbsMove(Axis _axis, float _pos);
	virtual bool axisRelMove(Axis _axis, float _pos);
	virtual bool sliceAbsMove(float _pos);
	virtual bool sliceRelMove(float _pos);
	virtual bool translationRelMove(float _pos);
	virtual bool translationAbsMove(float _pos);


	virtual bool readReadyStatus();
	virtual bool readSaveStatus();
	virtual bool readWaitNextScanStatus();
	virtual bool clearSaveFlag();
	virtual bool readIdleStatus();
	virtual std::map<Axis, float> readAxisSpeed();
	virtual float readAxisPostion(Axis _axis);
	virtual std::map<Axis, float> readAxisPostion();
	virtual std::map<Axis, float> readAxisWorkZero();
	virtual std::map<Axis, AxisCoordinateSwitchStatus> readAxisStatus();

	virtual bool startNextScan();
	virtual void getSystemStatus();
	virtual void getControlfSystemStatus();
	virtual void getAxisPosition();
	virtual void getAxisSpeed();
	virtual void getAxisWorkZero();

	virtual void stopGettingAxisPostion();

	virtual void setAxisSpeed(std::map<Axis, float>& _speed);
	virtual void setAxisWorkZero(std::map<Axis, float>& _workZero);
	
	virtual void sendToControl(int _cmd, char * _data, int _size, bool _consist);
	virtual void decodePackages(char* _package, int _size);
	virtual void restartLineDet(int _detNum); 
	virtual void switchRayDetCouple(int _couple);
	virtual void sendRayDetCoupleStatus(int _rayId, int _detId);
	virtual std::map<improtantStatus, bool>  readImportantStatus();
};

