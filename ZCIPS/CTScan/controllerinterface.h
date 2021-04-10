#pragma once
#include <QObJect>
#include "..\Public\headers\machineType.h"
#include <chrono>
#include <vector>
#include <map>
class QTimer;

struct AxisCoordinateSwitchStatus
{
	float coordinate;
	bool elp;
	bool sdp;
	bool sdn;
	bool eln;
	bool zeroFound;
};

class ControllerInterface : public QObject
{
	Q_OBJECT

signals:
	void netWorkStsSginal(bool sts);


public:
	enum improtantStatus
	{
		network,
		seekingZero,
		collide,
		changingRay,
		sliceThickAdjusting,
		rayDetCoupleNotSame,
		changingDet,
		busy
	};
protected:
	std::map<Axis, float> d_axisSpeed;
	std::map<Axis, float> d_axisPosition;
	std::map<Axis, float> d_axisWorkZero;
	const std::map<Axis, AxisData>& d_axisDataMap;
	std::map<improtantStatus, bool> d_importantStatus;
	bool d_connected;
	int d_timeoutCount;
	QTimer* d_timer;
	virtual void setConnectdSts() = 0;
	virtual bool sendCmd() = 0;
public:
	ControllerInterface(std::map<Axis, AxisData>& _axisDataMap);
	virtual ~ControllerInterface();
	
	virtual bool getConnected() = 0;
	virtual bool checkReadyToScan() = 0;

	virtual bool initialSend(SOCKET _sock) = 0;
	virtual bool stopAll() = 0;
	virtual bool initialiseController() = 0;
	virtual bool axisSeekZero(Axis _axis) = 0;
	virtual bool axisAbsMove(Axis _axis, float _pos) = 0;
	virtual bool axisRelMove(Axis _axis, float _pos) = 0;

	virtual bool translationRelMove(float _pos) = 0;
	virtual bool translationAbsMove(float _pos) = 0;

	virtual bool sliceAbsMove(float _pos) = 0;
	virtual bool sliceRelMove(float _pos) = 0;
	virtual bool readReadyStatus() = 0;
	virtual bool readSaveStatus() = 0;
	virtual bool readWaitNextScanStatus() = 0;
	virtual bool clearSaveFlag() = 0;
	virtual bool readIdleStatus() = 0;
	virtual std::map<Axis, float> readAxisSpeed() = 0;
	virtual float readAxisPostion(Axis _axis) = 0;
	virtual std::map<Axis, float> readAxisPostion() = 0;
	virtual std::map<Axis, AxisCoordinateSwitchStatus> readAxisStatus() = 0;
	virtual std::map<Axis, float> readAxisWorkZero() = 0;

	virtual bool startNextScan() = 0;
	virtual void getSystemStatus() = 0;
	virtual void getControlfSystemStatus() = 0;
	virtual void getAxisPosition() = 0;
	virtual void getAxisSpeed() = 0;
	virtual void getAxisWorkZero() = 0;

	virtual void stopGettingAxisPostion() = 0;

	virtual void setAxisSpeed(std::map<Axis, float>& _speed) = 0;
	virtual void setAxisWorkZero(std::map<Axis, float>& _workZero) = 0;
	virtual void sendToControl(int _cmd, char * _data, int _size, bool _consist) = 0;
	virtual void decodePackages(char* _package, int _size) = 0;
	virtual void restartLineDet(int _detNum) = 0;
	virtual void switchRayDetCouple(int _couple) = 0;
	virtual void sendRayDetCoupleStatus(int _rayId, int _detId) = 0;

	virtual std::map<improtantStatus, bool> readImportantStatus() = 0;
};

