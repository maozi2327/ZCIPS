#pragma once

#include <QObject>
#include <QtNetWork>
#include <future>
#include "CommandQueue.h"

class TcpClient : public QObject
{
	Q_OBJECT
private:
	QTcpSocket* d_tcpSocket;
	unsigned short d_serverPort;
	struct command
	{
		const char* _buffer;
		int _size;
	};
	bool d_connected;
	bool initialNetWork();
	void recvThread(std::promise<bool>& _promise);
	std::unique_ptr<std::promise<bool>> d_recvThreadPromisePtr;
	bool d_isRecvRunning;
	
	void sendThread(std::promise<bool>& _promise);
	std::unique_ptr<std::promise<bool>> d_sendThreadPromisePtr;
	bool d_isSendRunning;
	TheQueue<command> d_sendQueue;
public:
	TcpClient(QString& _hostAddress, quint16 _port, QObject *parent = Q_NULLPTR);
	~TcpClient();

	bool sendAsyn(const char* _buffer, int _size);
	int sendSyn(const char* _buffer, int _size);
	bool receive(char* _buffer, int _size);
};
