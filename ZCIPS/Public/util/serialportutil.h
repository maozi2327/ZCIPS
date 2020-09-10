#pragma once

#include <QObject>
#include <future>
#include "SerialPortInterface.h"
#include "CommandQueue.h"

class SerialPortUtil;
class SerialPort : public QObject
{
	Q_OBJECT
private:
	struct SendCommand
	{
		const char* _buffer;
		int _size;
	};
	struct RecvCommand
	{
		char* _buffer;
		int _size;
	};
	TheQueue<SendCommand> d_sendQueue;
	TheQueue<RecvCommand> d_receiveQueue;
	bool d_isOpen;
	SerialPortUtil* d_serialPortUtil;
	QSerialPort* d_serialPort;
	QString d_serialPortName;
	void recvThread(std::promise<bool>& _promise);
	std::unique_ptr<std::promise<bool>> d_recvThreadPromisePtr;
	bool d_isRecvRunning;
	void sendThread(std::promise<bool>& _promise);
	std::unique_ptr<std::promise<bool>> d_sendThreadPromisePtr;
	bool d_isSendRunning;

signals:
	void serialError(QString& _serialName);

public:
	SerialPort(SerialPortUtil* _serialPortUtil, QSerialPort* _port, QString _serialPortName);
	~SerialPort();

	bool closeSerialPort();
	bool sendAsyn(const char* _buffer, int _size);
	bool receive(char* _buffer, int _size);
};

class SerialPortUtil : public QObject, public SerialPortInterface
{
	Q_OBJECT

private:
	SerialPortUtil(QObject *parent = NULL);
	static SerialPortUtil* d_s_instance;
	SerialPortUtil(const SerialPortUtil&) {};
	SerialPortUtil& operator=(const SerialPortUtil&) { return *this; };
public:
	static SerialPortUtil* getHandle();
	~SerialPortUtil();
public:
	bool browseSerialPort(std::vector<std::pair<QString, bool>>& _portName);
	bool openSerialPort(const QString& _portName, QSerialPort::BaudRate, QSerialPort::DataBits, QSerialPort::StopBits
		, QSerialPort::FlowControl, QSerialPort::Parity);
	bool closeSerialPort(const QString& _portName);
	bool send(const char* _buffer, int _size, const QString& _portName);
	bool receive(char* _buffer, int _size, const QString& _portName);
public slots:
	bool onSerialError(QString& _serialName);
private:
	std::map<QString, SerialPort*> d_serialPortMap;
	QSerialPortInfo* d_serialPortInfo;
	QByteArray d_writeData;
	QTextStream d_standardOutput;
	qint64 d_bytesWritten = 0;
	QTimer d_timer;
};
