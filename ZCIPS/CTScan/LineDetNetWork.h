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
#define	CTOSRTBUF_NUM	20						//�������/���ͻ���������
#define	CTOSRTBUF_LEN	256						//�������/���ͻ���������

//ͨ�������ֽṹ
struct CMD_STRUCT
{
	unsigned int cmd_len;				//����λ����
	unsigned int cmd;					//�������
	unsigned int cmd_param;				//�������
	unsigned int respond_type;			//���ر�־
};

//�Լ�������

#define CMD_DETECTOR_TEST 0x0000		//̽��ϵͳ���
#define CMD_ARM_TEST 0x0a00				//ARM���
#define CMD_FPGA_TEST					//FPGA���

//����������
#define CMD_INTERNAL_COLLECT	0x0007	//���źŴ����ɼ�(����Ҫ��������)
#define CMD_CHANNEL_SELECT		0x0100	//FIFOͨ��ѡ��
#define CMD_CHANNEL_DEPTH_SET	0x0200	//FIFOͨ���������
#define CMD_START_CI			0x0800	//���������
#define CMD_NET_QUIT			0x0900	//�Ͽ�����

//����������
#define CMD_SET_DELAY_TIME	0x0001		//�����ӳ�ʱ��
#define CMD_SET_AMP_SIZE	0x0002		//���÷Ŵ���
#define CMD_SET_INT_TIME	0x0003		//���û���ʱ��
#define CMD_READ_DELAY_TIME 0x0004		//���ӳ�ʱ��
#define CMD_READ_AMP_SIZE	0x0005		//���Ŵ���
#define CMD_READ_INT_TIME	0x0006		//������ʱ��

//״̬/�����붨��					
enum 
{
	fLD_SUCCESS = 0x00,			//�ɹ�					0x00
	fLD_ARM_TEST_FAULT,										//��ʼ������̽�����������
	fLD_CHANNEL_SEL_FAULT,									//ͨ��ѡ��������
	fLD_CHANNEL_DEPTH_SET_FAULT,							//ͨ��������ý������
	fLD_START_CI_FAULT,										//��������忨�������
	fLD_DET_SELF_TEST_FAULT,								//̽���Լ�������
	fLD_SET_DELAY_TIME_FAULT,								//�����ӳ�ʱ��������
	fLD_SET_INT_TIME_FAULT,									//���û���ʱ��������
	fLD_SET_AMP_SIZE_FAULT,									//���÷Ŵ����������
	fLD_LOAD_DELAY_TIME_FAULT,								//�ϴ��ӳ�ʱ��������
	fLD_LOAD_INT_TIME_FAULT,								//�ϴ�����ʱ��������
	fLD_LOAD_AMP_SIZE_FAULT,								//�ϴ��Ŵ����������
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
			auto temp = ptr;
			delete[] ptr->d_item;
			ptr = ptr->d_next;
			delete temp;
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
		DATA,
		INTERNAL_COLLECT
	};
	bool d_connected;
	bool d_detInitSucceed;
	std::condition_variable d_con;
	std::mutex d_mutex;
	DataType d_recvType;
	unsigned int d_cmdType;
	unsigned int d_returnSize;           //�������ݴ�С

	unsigned int d_smallBoardNum;        //С��������
	unsigned short d_channelDepth;    //С����ͨ������64
	unsigned short d_ampSize;
	unsigned short d_sampleTime;
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
	unsigned long* d_internalCollectTempData;
	int d_bytesReceived;
	std::unique_ptr<Thread> d_netWorkCheckThread;
	//����̽����ÿ�η������ݸ�ʽ
	//									���ݳ���|
	//��һ��С����(�ֶȺ�|�����|64��ͨ��|У���)|�ڶ���С����|......|���һ��С����|
	//		  (----------------------��һ����������------------------------------------)
	//        ------------------------������������-----------------------------------
	//        -----------------------���һ����������-----------------------------------
	std::vector<unsigned int> d_blockModuleMap;
	int d_detNum;

	bool ChannelSelect();
	bool ChannelDepthSet();
	bool caculateChannel();
signals:
	void netWorkStatusSignal(int detId, bool sts);
public:
	bool recvServer_DATA();
	bool NetCheck();
	bool NetSetup();
	bool ARMTest();
	bool StartCI();
	bool FPGATest();
	bool DetectorTest();
	bool SetAmpSize(int _ampSize);
	bool SetSampleTime(int _sampleTime);
	bool SetDelayTime(int _delayTime);
	bool ReadAmpSize();
	bool ReadSampleTime();
	bool ReadDelayTime();
	bool setChannelMask(int _mask);
	bool setChannelDepth(int _depth);
	bool startExtTrigAcquire();
	bool startInternalTrigAcquire(int _mode, unsigned long* _data);
	bool stopInternalTrigAcquire();
	void stopAcquire(bool _finished);
	void DecodePackages(char* _buff, int _size);

	int getListItemSize();
	int getListItemNum();
	int getGraduationCount();
	int CollectUsefulData(char * _buff, int _size);
	LineDetList* getRowList();
	void clearRowList();

	bool getConnected();
	LineDetNetWork(unsigned short _port, unsigned short _fifoMask, unsigned short _channelDepth, unsigned short _delayTime,
		unsigned short _sampleTime, unsigned short _ampSize, std::vector<unsigned int> _blockModuleVec, int _detNum);
	~LineDetNetWork();
};

