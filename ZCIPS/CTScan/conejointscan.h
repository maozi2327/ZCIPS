#pragma once
#include "conescanInterface.h"

class ConeJointScan : public ConeScanInterface
{
public:
	ConeJointScan(Panel* _panel, ControllerInterface* _controller, PanelImageProcess* _ctTune, bool _bkgTuneFlag,
		bool _airTuneFlag, bool _defectTuneFlag, PanDetData _pandetData, const QString& _airFileNameA, const QString& _airFileNameB);
	~ConeJointScan();
	bool beginScan(int _graduation, int _framesPerGraduation, int _round, int _posTime, int _cycleTime,
		unsigned short _gainFactor, float _orientInc, float _slicePos, float _sod, float _sdd, float _detTranStart, float _detTranSpace);

protected:
	virtual void scanThread(); 
	virtual void createFileName(QString& _orgFileName, QString& _tunedFileName);
	virtual void sendCmdToController();
	virtual bool makeParameterText();

	//×ª¼¸È¦
	QString d_pathA;
	QString d_pathB;
	QString d_pathDest;
	QString d_airFileNameA;
	QString d_airFileNameB;
	QString d_currentOrgPath, d_currentTunedPath;
	QString d_timeName;


	int d_spliceProgress;
	int d_sampleCount;
	float d_detTransStart;
	float d_detTransSpace;
};

