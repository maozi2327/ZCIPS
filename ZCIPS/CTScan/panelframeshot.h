#pragma once
#include <QObject>
#include <functional>
#include "panelframesamleinterface.h"
#include <memory>

class DicomProcessor;

class PanelFrameShot : public PanelFrameSamleInterface
{
	Q_OBJECT

public:
	PanelFrameShot(Panel* _panel, PanelImageProcess* _panelImageProcess, bool bkgTuneFlag, 
		bool airTuneFlag, bool defectTuneFlag, QObject *parent);
	~PanelFrameShot();
	void beginPreview(int _cycleTime, unsigned short _gainFactor);
	void setDicomInfo(const QString& _objectName, const QString& _objectNumber);
protected:
	bool d_bkgTuneFlag;
	bool d_airTuneFlag;
	bool d_defectTuneFlag;
	void previewCallback(unsigned short* _image);
	virtual void saveFile();
	QString d_objectName;
	QString d_objectNumber;
signals:
	void imageAcquiredSignal(unsigned short* _image, int _width, int _height);
};
