#pragma once

#include <QObject>
class Panel;
class PanelImageProcess;

class PanelFrameSamleInterface : public QObject
{
	Q_OBJECT

public:
	PanelFrameSamleInterface(Panel* _panel, PanelImageProcess* _panelImageProcess, QObject *parent);
	~PanelFrameSamleInterface();
	void beginAcquire(int _frames, int _cycleTime, unsigned short _gainFactor);
	void setFileName(const QString& _orgName, const QString& _destPath);
	void stopAcquire();

protected:
	Panel* d_panel;
	PanelImageProcess* d_imageProcess;
	unsigned long* d_imageBuff;
	unsigned short* d_outputBuff;
	int d_width;
	int d_height;
	int d_frames;
	int d_frameCount;
	QString d_orgName;
	QString d_filePath;
	bool d_saveOrg;
	virtual void frameShotCallback(unsigned short* _image);
	virtual void saveFile() = 0;

signals:
	void imageAcquiredSignal(unsigned short* _image, int _width, int _height);
	void frameAcquiredSignal(int _framesAcquiredThisRound, int _framesThisRound, int _framesAcquiredAll, int _framesALL, QString _message);
};
