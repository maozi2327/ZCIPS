#pragma once
#include "panelframesamleinterface.h"

class PanelAirTune : public PanelFrameSamleInterface
{
public:
	PanelAirTune(Panel* _panel, PanelImageProcess* _panelImageProcess, QObject *parent = nullptr);
	~PanelAirTune();
	void setBkgFileName(const QString& _bkgFileName);
protected:
	QString d_bkgFileName;
	virtual void saveFile() override;
};

