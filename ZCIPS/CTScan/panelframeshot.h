#pragma once
#include <QObject>
#include <functional>
class Panel;
class PanelImageProcess;

class PanelFrameShot : public QObject
{
	Q_OBJECT

public:
	PanelFrameShot(Panel* _panel, PanelImageProcess* _panelImageProcess, QObject *parent);
	~PanelFrameShot();

	void beginAcquire(int _frames, int _cycleTime, unsigned short _gainFactor);
	void beginPreview(int _cycleTime);
private:
	Panel* d_panel;
	PanelImageProcess* d_imageProcess;
	int d_width;
	int d_height;
	int d_frames;
	int d_frameCount;
	void frameShotCallback(unsigned short* _image);
	void previewCallback(unsigned short* _image);
};
