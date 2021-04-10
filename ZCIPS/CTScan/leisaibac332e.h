#pragma once
#include "../../Public/headers/LTSMC.h"
#include <vector>
#include "../../Public/util/Thread.h"
#include <condition_variable>
#include <mutex>

#pragma comment(lib, "LTSMC.LIB")

struct LeiSaiAxisStatus
{
	float position;
	bool elPositive;
	bool elNegative;
	bool homed;
	bool checkDone;
};

struct LeiSaiStatus
{
	std::vector<LeiSaiAxisStatus> axesStatus;
};

// LeiSaiBac332E
class LeiSaiBac332E : public QObject
{
	Q_OBJECT

public:
	LeiSaiBac332E(const QString& _moduleFileDirectory);
	virtual ~LeiSaiBac332E();
	void initialAllAxes();
	bool setProfileUnit(int _axis);
	bool aixRelMove(int _axis, float _distance);
	bool aixAbsMove(int _axis, float _distance);
	bool stopAllAxes();
	bool homeAxis(int axis);
	bool homeAllAxes();
	bool setHomePosition(const std::vector<float>& position);
	void getHomePosition(std::vector<float>& zeros);
	LeiSaiStatus readStatus();
	bool getIdleStatus();
	float getZeroCoordinate(int _axis);
	bool getReadyStatus();
private:
	QString d_moduleFileFullName;
	unsigned long d_totalAxes;
	int d_connectNo;
	int d_axisSpeed;
	int d_acc;
	int d_dcc;
	long d_zeroPulse;
	std::vector<float> d_axisDistanceToPulse;
	std::vector<float> d_axisZeroPostion;
	LeiSaiStatus d_leiSaiStatus;
	bool d_quitThread;
	bool d_connected;
	bool loadZeroFile();
	bool saveZeroFile();
	std::atomic<bool> d_threadRun;
	QTimer* d_timer;
	std::unique_ptr<Thread> d_networkThread;
	std::condition_variable d_con;
	std::mutex d_mutex;
private slots:
	void timerFunctionSlot();
};


