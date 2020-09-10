#pragma once
#include <QObject>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <QTimer>
#include "../Public/util/logmacro.h"

class Thread;
class TcpServer;
#define	CTOSRTBUF_NUM	20						//定义接收/发送缓冲区个数
#define	CTOSRTBUF_LEN	256						//定义接收/发送缓冲区长度

//通信命令字结构
struct CMD_STRUCT
{
	unsigned int cmd_len;				//命令位长度
	unsigned int cmd;					//命令代码
	unsigned int cmd_param;				//命令参数
	unsigned int respond_type;			//返回标志
};

//自检类命令

#define CMD_DETECTOR_TEST 0x0000		//探测系统检测
#define CMD_ARM_TEST 0x0a00				//ARM检测
#define CMD_FPGA_TEST					//FPGA检测

//控制类命令
#define CMD_INTERNAL_COLLECT	0x0007	//内信号触发采集(不需要控制脉冲)
#define CMD_CHANNEL_SELECT		0x0100	//FIFO通道选择
#define CMD_CHANNEL_DEPTH_SET	0x0200	//FIFO通道深度设置
#define CMD_START_CI			0x0800	//启动传输板
#define CMD_NET_QUIT			0x0900	//断开网络

//参数类命令
#define CMD_SET_DELAY_TIME	0x0001		//设置延迟时间
#define CMD_SET_AMP_SIZE	0x0002		//设置放大倍数
#define CMD_SET_INT_TIME	0x0003		//设置积分时间
#define CMD_READ_DELAY_TIME 0x0004		//读延迟时间
#define CMD_READ_AMP_SIZE	0x0005		//读放大倍数
#define CMD_READ_INT_TIME	0x0006		//读积分时间

//状态/故障码定义					
enum 
{
	fLD_SUCCESS = 0x00,			//成功					0x00
	fLD_ARM_TEST_FAULT,										//初始化线阵探测器结果反馈
	fLD_CHANNEL_SEL_FAULT,									//通信选择结果反馈
	fLD_CHANNEL_DEPTH_SET_FAULT,							//通信深度设置结果反馈
	fLD_START_CI_FAULT,										//开启传输板卡结果反馈
	fLD_DET_SELF_TEST_FAULT,								//探测自检结果反馈
	fLD_SET_DELAY_TIME_FAULT,								//设置延迟时间结果反馈
	fLD_SET_INT_TIME_FAULT,									//设置积分时间结果反馈
	fLD_SET_AMP_SIZE_FAULT,									//设置放大倍数结果反馈
	fLD_LOAD_DELAY_TIME_FAULT,								//上传延迟时间结果反馈
	fLD_LOAD_INT_TIME_FAULT,								//上传积分时间结果反馈
	fLD_LOAD_AMP_SIZE_FAULT,								//上传放大倍数结果反馈
};

struct LineDetList
{
	unsigned long* d_item;
	LineDetList* d_prev;
	LineDetList* d_next;
};
struct RowList
{
private:
	std::mutex d_mutex;
	LineDetList* d_listHead;
	LineDetList* d_ptr;
	int d_size;
	int d_graduationNum;
public:
	RowList()
	{
		d_ptr = d_listHead = 0;
		d_size = 0;
		d_graduationNum = 0;
	}
	RowList(const RowList&) = delete;
	~RowList()
	{
		clear();
	}
	void push_back(unsigned long* _mem)
	{
		std::lock_guard<std::mutex> lock(d_mutex);

		if (d_size == 0)
		{
			d_ptr = new LineDetList;
			d_listHead = d_ptr;
			d_ptr->d_item = _mem;
			d_ptr->d_prev = 0;
			d_ptr->d_next = 0;
			++d_graduationNum;
		}
		else
		{
			d_ptr->d_next = new LineDetList;
			d_ptr->d_next->d_prev = d_ptr;
			d_ptr = d_ptr->d_next;
			d_ptr->d_item = _mem;
			d_ptr->d_next = 0;

			if (d_ptr->d_item[0] != d_ptr->d_prev->d_item[0])
				++d_graduationNum;
		}

		++d_size;
	}

	void clear()
	{
		std::lock_guard<std::mutex> lock(d_mutex);

		if (d_size == 0)
			return;

		auto ptr = d_listHead;

		while (ptr != nullptr)
		{
			delete[] ptr->d_item;
			ptr = ptr->d_next;
			delete ptr->d_prev;
		}

		d_size = 0;
		d_graduationNum = 0;
	}

	LineDetList* getList()
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		return d_listHead;
	}

	int getListSize()
	{
		std::lock_guard<std::mutex> lock(d_mutex);
		return d_size;
	}
};

class LineDetNetWork : public QObject
{
	Q_OBJECT
private:

	QTimer* d_timer;
	std::unique_ptr<TcpServer> d_server;
	enum DataType
	{
		CMD = 0,
		PARAMETER,
		DATA
	};
	bool d_connected;
	bool d_detInitSucceed;
	std::condition_variable d_con;
	std::mutex d_mutex;
	DataType d_recvType;
	unsigned int d_cmdType;
	unsigned int d_returnSize;           //返回数据大小

	unsigned int d_smallBoardNum;        //小板子数量
	unsigned short d_channelDepth;    //小板子通道数，64
	unsigned short d_ampSize;
	unsigned short d_intTime;
	unsigned short d_delayTime;
	unsigned short d_fifoMask;

	int d_dataSizePerPulse;
	int d_channelNum;
	RowList d_dataList;
	bool setParameterAfterConnect(SOCKET _sock);
	void pocessData(char* _package, int _size);

	bool d_isScanning;
	std::atomic<bool> d_deadThreadRunFlag;

	char* d_netWorkBuffer;
	int d_bytesReceived;
	std::unique_ptr<Thread> d_netWorkCheckThread;
	//线阵探测器每次发送数据格式
	//									数据长度|
	//第一个大板子(分度号|脉冲号|64个通道|校验和)|第二个大板子|......|最后一个大板子|
	//		  (----------------------第一个脉冲数据------------------------------------)
	//        ------------------------其他脉冲数据-----------------------------------
	//        -----------------------最后一个脉冲数据-----------------------------------
	std::vector<unsigned int> d_blockModuleMap;
	int d_detNum;
signals:
	void netWorkStatusSignal(int detId, bool sts);
public:
	bool recvServer_DATA();
	bool NetCheck();
	bool NetSetup();
	bool ARMTest();
	bool ChannelSelect();
	bool ChannelDepthSet();
	bool StartCI();
	bool FPGATest();
	bool DetectorTest();
	bool SetAmpSize(int _ampSize);
	bool SetIntTime(int _intTime);
	bool SetDelayTime(int _delayTime);
	bool ReadAmpSize();
	bool ReadIntTime();
	bool ReadDelayTime();
	bool startExtTrigAcquire();
	void setCIFinished(bool _finished);
	void DecodePackages(char* _buff, int _size);

	int getListItemSize();
	int getListItemNum();
	int getGraduationCount();
	int CollectUsefulData(char * _buff, int _size);
	LineDetList* getRowList();
	void clearRowList();

	bool getConnected();
	LineDetNetWork(unsigned short _port, unsigned short _fifoMask, unsigned short _channelDepth, unsigned short _delayTime,
		unsigned short _intTime, unsigned short _ampSize, std::vector<unsigned int> _blockModuleVec, int _detNum);
	~LineDetNetWork();
};

