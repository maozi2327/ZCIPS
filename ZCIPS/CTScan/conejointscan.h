#pragma once
#include "conescanInterface.h"
class ConeJointScan : public ConeScanInterface
{
public:
	ConeJointScan(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _ctTune, bool _bkgTuneFlag,
		bool _airTuneFlag, bool _defectTuneFlag, PanDetData _pandetData);
	~ConeJointScan();

protected:
	virtual void scanThread(); 
	virtual void createFileName(QString& _orgFileName, QString& _tunedFileName);
	virtual void sendCmdToController();

	//×ª¼¸È¦
	QString d_pathA;
	QString d_pathB;
	QString d_pathDest;
	int d_spliceProgress;
	int d_sampleCount;
	float d_detTransStart;
	float d_detTransSpace;
};

