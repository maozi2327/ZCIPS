#pragma once
#include "LineDetScanInterface.h"

class ControllerInterface;
class LineDetBkgTune :
	public LineDetScanInterface
{
public:
	LineDetBkgTune(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWork,
		const SetupData* _setupData, int _lineDetIndex, LineDetImageProcess* _lineDetImageProcess);
	~LineDetBkgTune();

private:
	
protected:
	virtual void sendCmdToControl() override;
	virtual bool caculateParemeterAndSetGenerialFileHeader() override;
	virtual void saveTempFile(LineDetList* _listHead) override;
	virtual void saveFile();
};

