// LeiSaiBac332E.cpp : 实现文件
//

#include "stdafx.h"
#include "leisaibac332e.h"
#include <functional>

const int totalAxis = 5;
const int pulsePerCircle[totalAxis] = {200, 200, 200, 200, 200};
const float ratio[totalAxis] = {1, 1, 3, 3, 5};
const float lead[totalAxis] = {5, 5, 5, 5, 5};
const float accTime = 0.1;
const float dccTime = 0.1;
const float axisSpeed = 400;
const int homeSpeedHigh = 400;
const int homeSpeedLow = 10;
const float sTime = 0;

QString zeroPositionFileName(QString::fromLocal8Bit("colimateZero.dat"));

#pragma pack(1)
struct ZeroPosition
{
	float axis[totalAxis];
};
#pragma pack()

LeiSaiBac332E::LeiSaiBac332E(const QString& _moduleFileDirectory) : d_totalAxes(totalAxis), d_connectNo(0), d_axisSpeed(axisSpeed), d_zeroPulse(0)
{
	d_moduleFileFullName = _moduleFileDirectory + zeroPositionFileName;
	d_threadRun = true;
	d_connected = false;
	d_leiSaiStatus.axesStatus.resize(d_totalAxes);
	d_axisDistanceToPulse.resize(d_totalAxes);
	d_axisZeroPostion.resize(d_totalAxes, 0);
	d_networkThread.reset(new Thread(std::bind(&LeiSaiBac332E::initialAllAxes, this), std::ref(d_threadRun)));
	d_networkThread->detach();

	for(int i = 0; i != totalAxis; ++i)
		d_axisDistanceToPulse[i] = lead[i] / (ratio[i] * pulsePerCircle[i]);

	d_timer = new QTimer();
	d_timer->start(200);
	connect(d_timer, &QTimer::timeout, this, &LeiSaiBac332E::timerFunctionSlot);
	loadZeroFile();
}

LeiSaiBac332E::~LeiSaiBac332E()
{
	d_quitThread = FALSE;
}

// LeiSaiBac332E 消息处理程序
void LeiSaiBac332E::timerFunctionSlot()
{
	if(d_connected)
		for(int axis = 0; axis != totalAxis; ++axis)
		{
			double position = 0;
			int iret = smc_get_position_unit(d_connectNo, axis, &position);

			if(iret != 0)
			{
				std::lock_guard<std::mutex> lock(d_mutex);
				d_connected = false;
				d_con.notify_all();
				break;
			}

			d_leiSaiStatus.axesStatus[axis].position = (position - d_zeroPulse) * d_axisDistanceToPulse[axis] + d_axisZeroPostion[axis];
			DWORD ioSts = smc_axis_io_status(d_connectNo, axis);
			d_leiSaiStatus.axesStatus[axis].elPositive = ioSts & 0x02 ? 1 : 0;
			d_leiSaiStatus.axesStatus[axis].elNegative = ioSts & 0x04 ? 1 : 0;
			WORD state = 0;
			smc_get_home_result(d_connectNo, axis, &state);
			d_leiSaiStatus.axesStatus[axis].homed = state;
			d_leiSaiStatus.axesStatus[axis].checkDone = smc_check_done(d_connectNo, axis);
		}
}

float LeiSaiBac332E::getZeroCoordinate(int _axis)
{
	return d_axisZeroPostion[_axis];
}

bool LeiSaiBac332E::getReadyStatus()
{
	bool ready = TRUE;

	for(int axis = 0; axis!= d_totalAxes; ++axis)
	{
		ready &= d_leiSaiStatus.axesStatus[0].homed;
	}

	return ready;
}

bool LeiSaiBac332E::getIdleStatus()
{
	bool idle = TRUE;

	for(int axis = 0; axis != totalAxis; ++axis)
		idle &= d_leiSaiStatus.axesStatus[axis].checkDone;

	return idle;
}

void LeiSaiBac332E::initialAllAxes()
{
	while(d_quitThread)
	{
		std::unique_lock<std::mutex> lock(d_mutex);
		d_con.wait(lock, [=]() { return !d_connected; });
		int err = smc_board_init(d_connectNo,2,"192.168.5.11",115200);
			
		if(err != 0)
			continue;

		err = nmcs_get_total_axes(d_connectNo, &d_totalAxes);

		for(int axis = 0; axis!= d_totalAxes; ++axis)
		{
			err = nmcs_set_axis_enable(d_connectNo, axis);
			err = smc_set_equiv(d_connectNo, axis, 1);
			err = smc_set_pulse_outmode(d_connectNo, axis, 0);
			err = smc_set_s_profile(d_connectNo, axis, 0, sTime);
			err = smc_set_profile_unit(d_connectNo, axis, 0, d_axisSpeed, accTime, dccTime, 0);
			nmcs_set_axis_enable(d_connectNo, axis);
		}
		d_connected = true;
	}
}

bool LeiSaiBac332E::setProfileUnit(int _axis)
{
	return 0;
}

bool LeiSaiBac332E::aixRelMove(int _axis, float _distance)
{
	smc_pmove_unit(d_connectNo, _axis, _distance / d_axisDistanceToPulse[_axis], 0);
	return 0;
}

bool LeiSaiBac332E::aixAbsMove(int _axis, float _distance)
{
	smc_pmove_unit(d_connectNo, _axis, (_distance - d_axisZeroPostion[_axis])/ d_axisDistanceToPulse[_axis], 1);
	return 0;
}

bool LeiSaiBac332E::stopAllAxes()
{
	for(int axis = 0; axis!= d_totalAxes; ++axis)
	{
		smc_stop(d_connectNo, axis, 1);
	}
	return 0;
}

bool LeiSaiBac332E::homeAxis(int axis)
{
	smc_set_home_profile_unit(d_connectNo, axis, homeSpeedLow, homeSpeedHigh, accTime, dccTime);//设置速度曲线
	smc_set_homemode(d_connectNo, axis, 0, 0, 17, 0);//设置回零方式
	smc_set_home_position_unit(d_connectNo, axis, TRUE, d_zeroPulse);
	smc_home_move(d_connectNo, axis);
	return 0;
}

bool LeiSaiBac332E::homeAllAxes()
{
	if(d_connected)
		for(int axis = 0; axis!= d_totalAxes; ++axis)
		{
			smc_set_home_profile_unit(d_connectNo, axis, homeSpeedLow, homeSpeedHigh, accTime, dccTime);//设置速度曲线
			smc_set_homemode(d_connectNo, axis, 0, 0, 17, 0);//设置回零方式
			smc_set_home_position_unit(d_connectNo, axis, TRUE, d_zeroPulse);
			smc_home_move(d_connectNo, axis);
		}
	return FALSE;
}

bool LeiSaiBac332E::setHomePosition(const std::vector<float>& position)
{
	d_axisZeroPostion = position;
	return saveZeroFile();
}
void LeiSaiBac332E::getHomePosition(std::vector<float>& zeros)
{
	zeros = d_axisZeroPostion;
}

LeiSaiStatus LeiSaiBac332E::readStatus()
{
	return d_leiSaiStatus;
}

bool LeiSaiBac332E::loadZeroFile()
{
	QFile file(d_moduleFileFullName);

	if (!file.open(QFile::ReadOnly))
		return false;

	unsigned short head = 0;
	file.read((char*)(&head), sizeof(unsigned short));

	if(head != 0x55AA)
	{
		file.remove();
		return FALSE;
	}
	
	ZeroPosition zeroPosition;
	file.read((char*)(&zeroPosition), sizeof(ZeroPosition));

	for(int axis = 0; axis != d_totalAxes; ++axis)
	{
		d_axisZeroPostion[axis] = zeroPosition.axis[axis];
	}
	
	file.close();
	return true;
}


bool LeiSaiBac332E::saveZeroFile()
{
	QFile file(d_moduleFileFullName);

	if (!file.open(QFile::WriteOnly))
		return false;

	unsigned short head = 0x55AA;
	file.write((char*)&head, sizeof(unsigned short));

	for(int axis = 0; axis != d_totalAxes; ++axis)
	{
		file.write((char*)&d_axisZeroPostion[axis], sizeof(float));
	}

	file.close();
	return TRUE;
}
