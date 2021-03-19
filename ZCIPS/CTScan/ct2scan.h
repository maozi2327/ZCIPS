#pragma once
#include "linedetscaninterface.h"
class Ct2Scan :
	public LineDetScanInterface
{
public:
	Ct2Scan(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWor, 
		const SetupData* _setupData, int _lineDetIndex);
	~Ct2Scan();

protected:
	int d_graduationCount;
	int d_imageAmount;
	virtual bool scanFinished();
	virtual void saveFile();
	virtual void sendCmdToControl();
	virtual bool setGenerialFileHeader();
public:
	bool setScanParameter(float _layer, int _matrix, float _view, int _sampleTime, 
		float _angle, int _imageAmount);
};

