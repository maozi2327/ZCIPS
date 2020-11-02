#pragma once
#include <QObject>
#include <functional>
class Panel;

class PanelFrameShot : public QObject
{
	Q_OBJECT

public:
	PanelFrameShot(Panel* _panel, QObject *parent);
	~PanelFrameShot();

	void beginAcquire(int _frames, int _cycleTime);
	void beginPreview(int _cycleTime);
private:
	Panel* d_panel;
	void frameShotCallback(unsigned short* _image);
	void previewCallback(unsigned short* _image);
};
