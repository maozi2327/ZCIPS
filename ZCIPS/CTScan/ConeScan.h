#pragma once
#include "conescanInterface.h"

class Panel;
class PanelImageProcess;

class ConeScan :
	public ConeScanInterface
{
public:
	ConeScan(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _ctDispose);
	~ConeScan();
	virtual void frameProcessCallback(unsigned short*);
	virtual bool stopScan();
	virtual bool intialise();
	
protected:
	virtual void scanThread();
	virtual bool loadAirData();

private:
	void sendCmdToController();
};

