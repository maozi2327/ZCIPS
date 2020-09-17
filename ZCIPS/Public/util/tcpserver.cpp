#include "stdafx.h"
#include "tcpserver.h"
#include <thread>

TcpServer::TcpServer(int _packetHeadSize, int _packetSizeLenth, int _packetSizePos
	, std::function<bool(SOCKET) > _sendDataCallBack
	, std::function<void(char*, int _size)> _dataHandlerCallBack
	, in_addr _hosetAddress, unsigned short _serverPort)
	: d_packetHeadSize(_packetHeadSize), d_packetSizeLenth(_packetSizeLenth), d_packetSizePos(_packetSizePos)
	, d_address(_hosetAddress), d_serverPort(_serverPort)
	, d_dataHandlerCallBack(_dataHandlerCallBack), d_sendInitiliseCallBack(_sendDataCallBack)
	, d_connected(false)
{
	d_sockaddr.sin_addr = d_address;
	d_sockaddr.sin_family = AF_INET;
	d_sockaddr.sin_port = htons(d_serverPort);
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	initialNetWorkForVariablePacketSize();
}

TcpServer::~TcpServer()
{
	closesocket(d_server);
	d_recvDeadThreadRun = false;
	d_sendDeadThreadRun = false;

	if (d_sendThreadPromisePtr)
		d_sendThreadPromisePtr->get_future().get();

	if (d_recvThreadPromisePtr)
		d_recvThreadPromisePtr->get_future().get();
}
bool TcpServer::initialNetWorkForVariablePacketSize()
{
	d_sendDeadThreadRun = true;
	d_sendThreadPromisePtr.reset(new std::promise<bool>);
	std::function<void()> sendThreadFun = std::bind(&TcpServer::sendThread, this, std::ref(*d_sendThreadPromisePtr));
	std::thread(sendThreadFun).detach();
	d_recvDeadThreadRun = true;
	d_recvThreadPromisePtr.reset(new std::promise<bool>);
	std::function<void()> recvThreadFun = std::bind(&TcpServer::recvThreadPacketHead, this, std::ref(*d_recvThreadPromisePtr));
	std::thread(recvThreadFun).detach();

	return true;
}
bool TcpServer::sendAsyn(const char* _buffer, int _size)
{
	if (!d_connected)
		return false;

	TcpServer::command cmd{ _buffer, _size };
	d_sendQueue.push(cmd);
	return true;
}

//TODO_DJ
//修改数据发送
//TODO_DJ
int TcpServer::sendSyn(const char* _buffer, int _size)
{
	int bytesSend = 0;

	if (d_connected)
	{
		while (bytesSend != _size)
		{
			int count = 0;

			if ((count = send(d_client, _buffer + bytesSend, _size - bytesSend, NULL)) == SOCKET_ERROR)
			{
				d_connected = false;
				break;
			}

			bytesSend += count;
		}

		return bytesSend;
	}

	return -1;
}
bool TcpServer::receive(char* _buffer, int _size)
{
	
	return true;
}

void TcpServer::recvThreadPacketHead(std::promise<bool>& _promise)
{
	_promise.set_value_at_thread_exit(true);
	char* buffer = new char[2u<<12];

	while (d_recvDeadThreadRun)
	{
		int byteRead = 0;

		while (d_connected)
		{
			int ret = recv(d_client, buffer, 2u << 12, NULL);

			if (ret == SOCKET_ERROR)
			{
				closesocket(d_client);
				d_connected = false;
				break;
			}

			if(ret > 0)
				d_dataHandlerCallBack(buffer, ret);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
	}

	delete[] buffer;
}
bool TcpServer::getConnected()
{
	return d_connected;
}
void TcpServer::reAccept()
{
	d_connected = false;
}
void TcpServer::sendThread(std::promise<bool>& _promise)
{
	_promise.set_value_at_thread_exit(true);

	while (d_sendDeadThreadRun)
	{
		if((d_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
			continue;

		if (bind(d_server, (sockaddr*)&d_sockaddr, sizeof(sockaddr)) == SOCKET_ERROR)
		{
			closesocket(d_server);
			continue;
		}
			
		if (listen(d_server, 10) == SOCKET_ERROR)
		{
			closesocket(d_server);
			continue;
		}

		while (d_sendDeadThreadRun)
		{
			sockaddr_in clientAddr;

			if ((d_client = accept(d_server, (sockaddr*)&clientAddr, NULL)) == SOCKET_ERROR)
				continue;

			d_connected = true;
			d_sendInitiliseCallBack(d_client);

			while (d_connected)
			{
				command cmd;

				if (!d_sendQueue.pop(cmd))
					continue;

				int bytesSend = 0;

				while (true)
				{
					int nRet = send(d_client, cmd._buffer + bytesSend, cmd._size - bytesSend, NULL);

					if (nRet == -1)
					{
						d_connected = false;
						closesocket(d_client);
						break;
					}

					bytesSend += nRet;

					if (bytesSend == cmd._size)
						break;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		}
	}
}
