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
class LineDetScanInterface : public QObject
{
	Q_OBJECT
protected:
	QTimer* d_timer;
	QString d_fileName;
	QString d_orgPath;
	QString d_filePath;
	QString d_installDirectory;
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
	float d_layer;
	float d_angle;
	int d_matrix;
	int d_allGraduationSample;
	float d_view;
	int d_sampleTime;
	int d_channelNum;

	std::chrono::steady_clock::time_point d_start_time;
	LineDetNetWork* d_lineDetNetWork;
	LineDetImageProcess* d_lineDetImageProcess;
	ControllerInterface* d_controller;
	std::unique_ptr<Thread> d_scanThread;
	RayType d_rayType;
	const SetupData* d_setupData;
	static ICT_HEADER23 d_ictHeader;
	virtual void scanThread() = 0;
	void saveOrgFile(LineDetList* _list);
	virtual bool setGenerialFileHeader();
	virtual void sendCmdToControl() = 0;
	void CalculateView_ValidDetector(float _diameter);
	virtual bool canScan() = 0;
	virtual int loadTempFile();
signals:
	void signalGraduationCount(int _count);
	LOGSIGNAL
public:
	LineDetScanInterface(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork, 
		const SetupData* _setupData, int _lineDetIndex);
	virtual ~LineDetScanInterface();
	void setFileName(const QString& _fileName) { d_fileName = _fileName; };
	void setFilePath(const QString& _filePath) { d_filePath = _filePath; };
	void setOrgPath(const QString& _orgPath) { d_orgPath = _orgPath; };
	virtual bool beginScan() = 0;
	virtual void stopScan() = 0;
};