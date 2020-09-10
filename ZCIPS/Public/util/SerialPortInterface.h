#pragma once
#include <QSerialPortInfo>
#include <QSerialPort>
#include <vector>
#include <utility>
#include <memory>
class SerialPortInterface
{
public:
	virtual bool browseSerialPort(std::vector<std::pair<QString, bool>>& _portName) = 0;
	virtual bool openSerialPort(const QString& _portName, QSerialPort::BaudRate, QSerialPort::DataBits, QSerialPort::StopBits
		, QSerialPort::FlowControl, QSerialPort::Parity) = 0;
	virtual bool closeSerialPort(const QString& _portName) = 0;
	virtual bool send(const char* _buffer, int _size, const QString& _portName) = 0;
	virtual bool receive(char* _buffer, int _size, const QString& _portName) = 0;
};
class PanelFactory
{
public:
	static SerialPortInterface* getHandle();
};
