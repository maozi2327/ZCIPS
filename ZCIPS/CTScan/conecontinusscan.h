#pragma once
#include "conescanInterface.h"
class PanelImageProcess;
class ConeContinusScan :
	public ConeScanInterface
{
private:
	int d_graduationCount;
public:
	ConeContinusScan(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _ctDispose);
	~ConeContinusScan();

	virtual bool stopScan();
	virtual void scanThread(std::promise<bool>& _promise);
};

