#pragma once
#include <memory>
#include <mutex>
#include "../Public/util/logmacro.h"
#include "../PanelDll/panel.h"
#include "../Public/headers/panelimageprocess.h"
#include "../Public/headers/commandqueue.h"
#include "../Public/util/thread.h"

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
	int d_graduationCount;
	int d_round;
	int d_imageProcessSleep;
	int d_sampleTime;
	int d_frameCount;
	size_t d_frameSize;
	float d_orientInc;
	std::vector<QString> d_parameterText;

	mutable std::mutex d_hmtxQ;
	std::list<unsigned short*> d_imageList;
	std::condition_variable d_con;
	Panel* d_panel;
	ControllerInterface* d_controller;
	PanelImageProcess* d_imageProcess;
	virtual void frameProcessCallback(unsigned short* _image);
	std::unique_ptr<Thread> d_scanThread;
	virtual void scanThread() = 0;
	std::atomic<bool> d_deadThreadRun;
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
	virtual void setGraduation(int _graduation) { d_graduation = _graduation; };
	virtual bool stopScan() = 0;
	virtual bool intialise() = 0;
	virtual bool beginScan();
	virtual void getScanProgress(int& _thisRound, int& _allProgress, QString& imagesCollectedAndSpaceOccupied);
signals:
	LOGSIGNAL
};