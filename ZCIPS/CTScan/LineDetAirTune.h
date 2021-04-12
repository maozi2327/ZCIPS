#pragma once
#include "LineDetScanInterface.h"
class LineDetAirTune :
	public LineDetScanInterface
{
protected:
	virtual bool caculateParemeterAndSetGenerialFileHeader();
	virtual void sendCmdToControl();
	virtual void saveTempFile(LineDetList* _listHead) override;
public:
	LineDetAirTune(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWor,
		const SetupData* _setupData, int _lineDetIndex, LineDetImageProcess* _lineDetImageProcess);
	~LineDetAirTune();
	virtual void stopScan();
	virtual void saveFile();
};

