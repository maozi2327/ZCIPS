#pragma once
#include "linedetscaninterface.h"
class DrScan :	public LineDetScanInterface
{
public:	
	DrScan(ControllerInterface* _controller, LineDetNetWork* _lineDetNetWor, 
		const SetupData* _setupData, int _lineDetIndex, LineDetImageProcess* _lineDetImageProcess);
	~DrScan();

	bool caculateParemeterAndSetGenerialFileHeader();
	bool setScanParameter(float _layers, int _matrix, float _view,
		int _sampleTime, float _angle, float _layerLenth, float _layerSpace);
protected:
	float d_layerStartPoint;
	float d_layerLenth;
	void sendCmdToControl();
	void saveFile();
	virtual void saveTempFile(LineDetList* _listHead);

	float d_layerSpace;
};

