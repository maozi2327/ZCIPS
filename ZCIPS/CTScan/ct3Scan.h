#pragma once
#include <vector>
#include "LineDetScanInterface.h"
#include "../Public/headers/setupdata.h"

struct LineDetList;
class CT3Scan :	public LineDetScanInterface
{
private:
	unsigned int d_correctFlag;
	static std::chrono::minutes d_intervalForSaveTempFile;
	unsigned int d_tempFileNum;
	QString d_tempFileDir;
	std::vector<QString> d_tempFileVec;
	bool d_ecqualLayer;
	std::vector<float> d_layer;
protected:
	//void scanThread();
	void sendCmdToControl();
	virtual void saveFile();
	virtual bool caculateParemeterAndSetGenerialFileHeader();
	virtual bool canScan();
	virtual bool scanFinished();
	void saveTempFile(LineDetList* _listHead);
public:
	CT3Scan(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork, 
		const SetupData* _setupData, int _lineDetIndex, LineDetImageProcess* _lineDetImageProcess);
	~CT3Scan();
	bool setScanParameter(std::vector<float> _layer, int _matrix, float _view, int _sampleTime, float _angle, bool _ecqualLayer);
};

