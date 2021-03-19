#pragma once
#include "linedetscaninterface.h"
class DrScan :	public LineDetScanInterface
{
public:	
	DrScan(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWor, 
		const SetupData* _setupData, int _lineDetIndex);
	~DrScan();

	bool setGenerialFileHeader();
	bool setScanParameter(float _layers, int _matrix, float _view,
		int _sampleTime, float _angle, float _layerLenth);
protected:
	float d_layerLenth;
	void sendCmdToControl();
	void saveFile();
};

