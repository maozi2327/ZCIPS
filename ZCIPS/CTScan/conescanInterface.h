#pragma once
#include <memory>
#include <mutex>
#include "../Public/util/logmacro.h"
#include "../PanelDll/panel.h"
#include "../PanelImageProcess/panelimageprocess.h"
#include "../Public/headers/commandqueue.h"
#include "../Public/util/thread.h"

class PanelRawData
{
private:
	int* d_useCount;
	unsigned short* d_rawData;
	int d_size;

public:
	PanelRawData(unsigned short* d_image, int _size)
		: d_rawData(d_image), d_size(_size), d_useCount(new int(1))
	{
	};

	PanelRawData(const PanelRawData& _in) 
		: d_rawData(_in.d_rawData), d_size(_in.d_size), d_useCount(_in.d_useCount)
	{
		++*d_useCount;
	};

	PanelRawData& operator=(const PanelRawData& _in)
	{
		++*_in.d_useCount;

		if (--*d_useCount == 0)
		{
			if (d_rawData)
			{
				delete[] d_rawData;
				d_rawData = nullptr;
			}
		}

		d_size = _in.d_size;
		d_rawData = _in.d_rawData;
		d_useCount = _in.d_useCount;
		return* this;
	};

	~PanelRawData()
	{
		if(--*d_useCount == 0)
			if (d_rawData)
			{
				delete[] d_rawData;
				d_rawData = nullptr;
			}
	};
};

class ControllerInterface;
class ConeScanInterface : public QObject
{
	Q_OBJECT
private:
	;
protected:
	QString d_fileFolder;
	QString d_fileName;
	QString d_parameterFileName;
	bool d_isSaveOrg;
	bool d_isDispose;
	bool d_bkgFlag;
	bool d_airFlag;
	bool d_defectFlag;
	bool d_averageFlag;
	bool d_orgFlag;
	int d_width;
	int d_height;
	int d_framesPerGraduation;
	int d_graduation;
	std::atomic<int> d_graduationCount;
	int d_round;
	int d_imageProcessSleep;
	int d_posTime;
	int d_frameCount;
	size_t d_frameSize;
	float d_orientInc;
	std::vector<QString> d_parameterText;
	TheQueue<PanelRawData> d_imageList1;

	std::atomic<bool> d_deadThreadRun;
	std::atomic<bool> d_imageProcessThreadFlag;
	TheQueue<unsigned short*> d_imageList;
	std::condition_variable d_con;
	mutable std::mutex d_hmtxQ;

	Panel* d_panel;
	ControllerInterface* d_controller;
	PanelImageProcess* d_imageProcess;
	virtual void frameProcessCallback(unsigned short* _image);
	std::unique_ptr<Thread> d_scanThread;
	virtual void scanThread() = 0;
	std::unique_ptr<Thread> d_imageProcessThread;
	virtual void imageProcessThread();
	bool d_imageProcessThreadRun;
	virtual bool canScan();
	virtual void sendCmdToController() = 0;
	virtual bool loadBkgData();
	virtual bool loadAirData() = 0;
	virtual bool writeParameterFile();
	virtual bool makeParameterText();
	virtual bool loadDefectData();
	virtual bool saveFile(unsigned short* _image);
	bool checkMemory();
public:
	ConeScanInterface(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _ctDispose);
	virtual ~ConeScanInterface();

	virtual void setFileName(QString& _fileFolder, QString& _name);
	virtual void setDisposeFlag(bool _bkgFlag, bool _airFlag, bool _defectFlag, bool _averageFlag);
	virtual bool stopScan();
	virtual bool intialise() = 0;
	virtual bool beginScan(int _graduation, int _framesPerGraduation, int _posTime, int _cycleTime, float _orientInc);
	virtual void getScanProgress(int& _thisRound, int& _allProgress, QString& imagesCollectedAndSpaceOccupied);
signals:
	LOGSIGNAL
};