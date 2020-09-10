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
	void saveOrgFile(LineDetList* in_list);
	virtual bool setGenerialFileHeader();
	virtual void sendCmdToControl() = 0;
	void CalculateView_ValidDetector(float in_diameter);
	virtual bool canScan() = 0;
	virtual int loadTempFile();
signals:
	void signalGraduationCount(int in_count);
	LOGSIGNAL
public:
	LineDetScanInterface(ControllerInterface* in_controller, LineDetNetWork* in_lineDetNetWork, 
		const SetupData* in_setupData, int in_lineDetIndex);
	virtual ~LineDetScanInterface();
	void setFileName(QString& in_fileName, QString& in_orgPth, QString& in_filePth) { d_fileName = in_fileName; };
	virtual bool beginScan() = 0;
	virtual void stopScan() = 0;
};