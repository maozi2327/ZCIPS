#pragma once
#include <vector>
#include "LineDetScanInterface.h"
#include "../Public/headers/setupdata.h"
struct LineDetList;
class CT3Scan :
	public LineDetScanInterface
{
private:
	QString d_destFileName;
	unsigned int d_correctFlag;
	static std::chrono::minutes d_intervalForSaveTempFile;
	unsigned int d_tempFileNum;
	QString d_tempFileDir;
	std::vector<QString> d_tempFileVec;
protected:
	void scanThread();
	void sendCmdToControl();
	virtual void saveFile();
	virtual bool setGenerialFileHeader();
	virtual bool canScan();
	void saveTempFile(LineDetList* in_listHead);
public:
	CT3Scan(ControllerInterface* in_controller, LineDetNetWork* in_lineDetNetWork, 
		const SetupData* in_setupData, int in_lineDetIndex);
	~CT3Scan();
	bool setScanParameter(float in_layers, int in_matrix, float in_view,
		int in_sampleTime, float in_angle);
	virtual void stopScan();
	virtual bool beginScan();
};

