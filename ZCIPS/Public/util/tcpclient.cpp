#include "stdafx.h"
#include "tcpclient.h"

TcpClient::TcpClient(QString& _hostAddress, quint16 _port, QObject *parent)
	: QObject(parent)
	, d_tcpSocket(new QTcpSocket)
{
	d_tcpSocket->connectToHost(_hostAddress, _port);
}

TcpClient::~TcpClient()
{
	d_isRecvRunning = true;
	d_isSendRunning = true;
	if (d_recvThreadPromisePtr)
		d_recvThreadPromisePtr->get_future().get();

	if (d_sendThreadPromisePtr)
		d_sendThreadPromisePtr->get_future().get();
}

bool TcpClient::sendAsyn(const char* _buffer, int _size)
{
	TcpClient::command cmd{ _buffer, _size };
	d_sendQueue.push(cmd);
	return true;
}

bool TcpClient::initialNetWork()
{
	d_sendThreadPromisePtr.reset(new std::promise<bool>);
	std::function<void()> sendThreadFun = std::bind(&TcpClient::sendThread, this, std::ref(*d_sendThreadPromisePtr));
	std::thread(sendThreadFun).detach();
	d_recvThreadPromisePtr.reset(new std::promise<bool>);
	std::function<void()> recvThreadFun = std::bind(&TcpClient::recvThread, this, std::ref(*d_recvThreadPromisePtr));
	std::thread(recvThreadFun).detach();

	return true;
	//return d_tcpServer->listen(_address, _serverPort);
}

int TcpClient::sendSyn(const char* _buffer, int _size)
{
	return 0;
}

bool TcpClient::receive(char* _buffer, int _size)
{
	return 0;
}
void TcpClient::sendThread(std::promise<bool>& _promise)
{
	_promise.set_value_at_thread_exit(true);

	while (d_isSendRunning)
	{
		command cmd;
		int byteSend = 0;

		if(!d_sendQueue.pop(cmd))
			continue;

		while (d_connected)
		{
			int nRet = d_tcpSocket->write(cmd._buffer + byteSend, cmd._size - byteSend);

			if (nRet == -1)
			{
				d_tcpSocket->close();
				break;
			}

			byteSend += nRet;

			if (byteSend == cmd._size)
				break;
		}
	}
}
void TcpClient::recvThread(std::promise<bool>& _promise)
{

}