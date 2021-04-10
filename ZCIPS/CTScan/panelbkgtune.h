#pragma once
#include "panelframesamleinterface.h"

class PanelBkgTune : public PanelFrameSamleInterface
{
public:
	PanelBkgTune(Panel* _panel, PanelImageProcess* _panelImageProcess, QObject *parent = nullptr);
	~PanelBkgTune();

protected:
	virtual void saveFile();
};

