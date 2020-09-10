#include "stdafx.h"
#include "serialportutil.h"
#include <thread>

//-----------------SerialPortUtil::--------------------------------------

SerialPortUtil* SerialPortUtil::d_s_instance;
SerialPortUtil::SerialPortUtil(QObject *parent)
	: QObject(parent)
{
	
}

SerialPortUtil * SerialPortUtil::getHandle()
{
	if (d_s_instance == nullptr)
		d_s_instance = new SerialPortUtil();

	return d_s_instance;
}

SerialPortUtil::~SerialPortUtil()
{
	
}

bool SerialPortUtil::browseSerialPort(std::vector<std::pair<QString, bool>>& _portName)
{
	const auto infos = QSerialPortInfo::availablePorts();
	_portName.clear();

	QSerialPort* originPort = new QSerialPort();
	
	for (const QSerialPortInfo &info : infos)
	{
		originPort->setPortName(info.portName());
		_portName.push_back({ info.portName(), originPort->isOpen()});
	}

	return true;
}

bool SerialPortUtil::openSerialPort(const QString& _portName, QSerialPort::BaudRate _baudRate
	, QSerialPort::DataBits _dataBits, QSerialPort::StopBits _stopBits
	, QSerialPort::FlowControl _flowControl, QSerialPort::Parity _parity)
{
	if (d_serialPortMap.find(_portName) != d_serialPortMap.end())
	{
		QSerialPort* originPort = new QSerialPort();
		originPort->setPortName(_portName);
		originPort->setBaudRate(_baudRate);
		originPort->setStopBits(_stopBits);
		originPort->setFlowControl(_flowControl);
		originPort->setParity(_parity);

		if (originPort->open(QIODevice::WriteOnly))
		{
			SerialPort* port = new SerialPort(this, originPort, _portName);
			d_serialPortMap.insert({ _portName, port });
			return true;
		}
	}

	return false;
}

bool SerialPortUtil::closeSerialPort(const QString & _portName)
{
	if (d_serialPortMap.find(_portName) != d_serialPortMap.end())
	{
		d_serialPortMap[_portName]->closeSerialPort();
		d_serialPortMap.erase(_portName);
	}

	return true;
}

bool SerialPortUtil::send(const char * _buffer, int _size, const QString & _portName)
{
	if (d_serialPortMap.find(_portName) != d_serialPortMap.end())
		if (d_serialPortMap[_portName]->sendAsyn(_buffer, _size))
			return true;

	return false;
}


bool SerialPortUtil::receive(char * _buffer, int _size, const QString & _portName)
{
	if (d_serialPortMap.find(_portName) != d_serialPortMap.end())
		d_serialPortMap[_portName]->receive(_buffer, _size);

	return true;
}

bool SerialPortUtil::onSerialError(QString& _serialName)
{
	closeSerialPort(_serialName);
	return true;
}
//-----------------SerialPortUtil::SerialPort----------------------------
SerialPort::SerialPort(SerialPortUtil* _serialPortUtil, QSerialPort* _port, QString _serialPortName)
	: d_serialPortUtil(_serialPortUtil)
{
	d_isOpen = true;
	d_isSendRunning = true;
	d_isRecvRunning = true;
	d_sendThreadPromisePtr.reset(new std::promise<bool>());
	std::function<void()> sendFunc = std::bind(&SerialPort::sendThread, this, std::ref(*d_sendThreadPromisePtr));
	std::thread(sendFunc).detach();
	
	d_recvThreadPromisePtr.reset(new std::promise<bool>());
	std::function<void()> recvFunc = std::bind(&SerialPort::recvThread, this, std::ref(*d_recvThreadPromisePtr));
	std::thread(recvFunc).detach();

	connect(this, &SerialPort::serialError, d_serialPortUtil, &SerialPortUtil::onSerialError);
}


SerialPort::~SerialPort()
{
	d_isSendRunning = false;
	d_isRecvRunning = false;
	d_sendThreadPromisePtr->get_future().get();
	d_recvThreadPromisePtr->get_future().get();
}

bool SerialPort::closeSerialPort()
{
	d_isOpen = false;
	d_isRecvRunning = false;
	d_isSendRunning = false;
	d_serialPort->close();
	return true;
}

bool SerialPort::sendAsyn(const char * _buffer, int _size)
{
	if (d_isOpen)
	{
		SendCommand cmd = { _buffer, _size };
		d_sendQueue.push(cmd);
		return true;
	}
	else
		return false;
}

bool SerialPort::receive(char * _buffer, int _size)
{
	RecvCommand cmd;

	if(!d_receiveQueue.pop(cmd))
		return false;
	
	_buffer = cmd._buffer;
	_size = cmd._size;
	return true;
}

void SerialPort::sendThread(std::promise<bool>& _promise)
{
	_promise.set_value_at_thread_exit(true);

	while (d_isSendRunning)
	{
		SendCommand cmd;
		int byteSend = 0;
		
		if(!d_sendQueue.pop(cmd))
			continue;

		while (d_isOpen)
		{
			byteSend = d_serialPort->write(cmd._buffer);

			if (byteSend != cmd._size)
			{
				d_isOpen = false;
				emit serialError(d_serialPortName);
				break;
			}
		}
	}
}

void SerialPort::recvThread(std::promise<bool>& _promise)
{
	_promise.set_value_at_thread_exit(true);

	while (d_isSendRunning)
	{
		while (d_serialPort->waitForReadyRead(1000))
		{
			QByteArray byteArray = d_serialPort->readAll();
			
			if (byteArray.size() == 0)
			{
				d_isOpen = false;
				emit serialError(d_serialPortName);
				break;
			}

			char* buf = new char[byteArray.size() + 1];
			RecvCommand cmd = { buf, byteArray.size() };
			d_receiveQueue.push(cmd);
		}
	}
}