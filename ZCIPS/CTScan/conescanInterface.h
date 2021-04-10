#pragma once
#include <memory>
#include <mutex>
#include "../Public/headers/setupdata.h"
#include "../Public/util/logmacro.h"
#include "../PanelImageProcess/panelimageprocess.h"
#include "../Public/headers/commandqueue.h"
#include "../Public/util/thread.h"
#include <list>

class Panel;
struct PanelRawData
{
	unsigned short* d_rawData;
	size_t d_size;
	int d_implCount;

	PanelRawData() 
		: d_rawData(nullptr), d_size(0)
	{

	}

	PanelRawData(unsigned short* d_image, size_t _size)
		: d_rawData(d_image), d_size(_size)
	{
		++d_implCount;
		d_rawData = new unsigned short[_size / sizeof(unsigned short)];
		memcpy(d_rawData, d_image, _size);
	};

	PanelRawData(const PanelRawData& _in) 
		: d_rawData(_in.d_rawData), d_size(_in.d_size)
	{
		++d_implCount;
		d_rawData = _in.d_rawData;
		d_size = _in.d_size;
	};

	PanelRawData& operator=(const PanelRawData& _in)
	{
		if (d_rawData != _in.d_rawData)
		{
			d_rawData = _in.d_rawData;
			d_size = _in.d_size;
			++d_implCount;
		}
		return *this;
	};

	~PanelRawData()
	{
		if (--d_implCount == 0 && d_rawData != nullptr)
			delete[] d_rawData;
	};
};


class ControllerInterface;
class ConeScanInterface : public QObject
{
	Q_OBJECT
private:
	;
protected:
	//传入的保存org图片文件夹的全路径名字，
	QString d_orgName;
	//传入的保存校正后文件夹的文件夹的全路径名字
	QString d_tunedFilePath;
	//完整的图片路径
	QString d_totalOrgDirectory;

	QString d_fileFolder;
	QString d_fileName;
	static QString d_parameterFileName;

	//不包含上级路径的图片文件夹名字
	QString d_pureDirectoryName;

	bool d_isSaveOrg;
	bool d_isTune;

	bool d_bkgTuneFlag;
	bool d_airTuneFlag;
	bool d_defectTuneFlag;

	bool d_averageFlag;
	bool d_saveOrg;

	int d_width;
	int d_height;
	int d_framesPerGraduation;
	int d_graduation;
	int d_graduationCount;
	int d_round;
	int d_imageProcessSleep;
	int d_posTime;
	int d_frameCount;
	size_t d_frameSize;
	float d_orientInc;
	int d_cycleTime;
	int d_gainFactor;
	float d_slicePos;
	float d_sod;
	float d_sdd;

	int d_panelBlockPixelLeft;
	int d_panelBlockPixelRight;

	//扫描时间，秒数?
	int d_scanTime;

	int d_scanProc;
	std::list<QString> d_parameterText;
	std::atomic<bool> d_imageProcessThreadRun;
	std::atomic<bool> d_scanThreadRun;
	std::atomic<bool> d_imageProcessThreadFlag;
	TheQueue<unsigned short*> d_imageList;
	std::condition_variable d_con;
	mutable std::mutex d_hmtxQ;

	Panel* d_panel;
	ControllerInterface* d_controller;
	PanelImageProcess* d_imageProcess;
	const PanDetData d_pandetData;
	virtual void frameProcessCallback(unsigned short* _image);
	std::unique_ptr<Thread> d_scanThread;
	virtual void scanThread() = 0;
	std::unique_ptr<Thread> d_imageProcessThread;
	virtual void imageProcessThread();
	virtual bool canScan();
	virtual void sendCmdToController() = 0;
	virtual bool loadBkgData();
	virtual bool loadAirData() = 0;
	virtual bool writeParameterFile(const QString& _path);
	virtual bool makeParameterText();
	virtual bool saveFile(unsigned short* _image);
	virtual void createFileName(QString& _orgFileName, QString& _tunedFileName);
	bool checkMemory();
public:
	ConeScanInterface(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _ctTune, bool _bkgTuneFlag, 
		bool _airTuneFlag, bool _defectTuneFlag, PanDetData _pandetData);
	virtual ~ConeScanInterface();

	virtual void setFileName(const QString& _orgName, const QString& _destPath);
	virtual void setTuneFlag(bool _bkgFlag, bool _airFlag, bool _defectFlag, bool _averageFlag);
	virtual bool stopScan();
	virtual bool intialise() = 0;
	virtual bool beginScan(int _graduation, int _framesPerGraduation, int _round, int _posTime, int _cycleTime, 
		unsigned short _gainFactor, float _orientInc, float _slicePos, float _sod, float _sdd);
	virtual void getScanProgress(int& _thisRound, int& _allProgress, QString& imagesCollectedAndSpaceOccupied);
signals:
	void scanProgressSignal(int _framesAcquiredThisRound, int _framesThisRound, int _framesAcquiredAll, int _framesALL, QString message);
	void imageAcquiredSignal(unsigned short* _image, int _width, int _height);
	LOGSIGNAL
};