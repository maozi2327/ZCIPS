#pragma once
#include <QTimer>
#include "ictheader.h"
#include "setupdata.h"
#include "ctrldata.h"
#include "../Public/util/logmacro.h"
#include <memory>
#include <chrono>

class Thread;
class LineDetNetWork;
class LineDetImageProcess;
class ControllerInterface;
class LineDetList;
class FileDB;

class LineDetScanInterface : public QObject
{
	Q_OBJECT
protected:
	QTimer* d_timer;
	QString d_fileName;
	
	//org全路径名
	QString d_orgName;

	//校正后文件路径
	QString d_filePath;
	QString d_installDirectory;
	QString d_airFile;
	QString d_bkgFile;
	std::atomic<bool> d_deadThreadRun;

	int	d_AccIndex;
	int d_accFrecIndex;
	int	d_TubeIndex;
	int	d_lineDetIndex;
	float d_SOD;
	float d_SDD;
	float d_colimateSize;
	float d_layerThickness;
	float d_viewDiameter;
	float d_angle;
	int d_matrix;
	int d_currentScanTotalSamples;
	int d_allScanTotalSamples;
	float d_view;
	int d_sampleTime;
	int d_channelNum;
	int d_imageScaned;
	int d_currentSamples;
	int d_samplesBefore;
	bool d_saveOrg;

	std::chrono::steady_clock::time_point d_start_time;
	static std::chrono::minutes d_intervalForSaveTempFile;
	LineDetNetWork* d_lineDetNetWork;
	LineDetImageProcess* d_lineDetImageProcess;
	ControllerInterface* d_controller;
	FileDB* d_fileDB;
	std::unique_ptr<Thread> d_scanThread;
	RayType d_rayType;
	const SetupData* d_setupData;
	const ConfigData* d_configData;
	static ICT_HEADER23 d_ictHeader;
	virtual void scanThread();
	void saveOrgFile(LineDetList* _list, const QString& _fileName);
	void saveOrgFile();
	virtual bool scanFinished();
	virtual bool caculateParemeterAndSetGenerialFileHeader();
	virtual void sendCmdToControl() = 0;
	void CalculateView_ValidDetector(float _diameter);
	virtual bool canScan();
	virtual int loadTempFile();
	virtual void saveTempFile(LineDetList* _listHead) = 0;
	virtual void saveFile() = 0;
signals:
	void scanProgressSignal(int _graduationAcquiredThisRound, int _graduationThisRound, int _graduationAcquiredAll, int _graduationALL, QString message);
	void scanThreadQuitSignal(int _sts);
	LOGSIGNAL

public:
	LineDetScanInterface(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork, 
		const SetupData* _setupData, int _lineDetIndex, LineDetImageProcess* _lineDetImageProcess);
	virtual void setFileName(const QString& _orgName, const QString& _destPath);
	virtual ~LineDetScanInterface();
	virtual bool beginScan();
	virtual void stopScan();
};