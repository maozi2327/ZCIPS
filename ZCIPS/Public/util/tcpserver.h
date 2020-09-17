#pragma once
#include <WinSock.h>
#include <future>
#include <functional>
#include "CommandQueue.h"
#pragma comment(lib, "Ws2_32.lib")

class TcpServer
{
public:
	TcpServer(int _packetHeadSize, int _packetSizeLenth, int _packetSizePos
		, std::function<bool(SOCKET)> _sendDataCallBack, std::function<void(char*, int _size)> _recvDataCallBack
		, in_addr _hosetAddress, unsigned short _serverPort);
	virtual ~TcpServer();
	bool sendAsyn(const char* _buffer, int _size);
	int sendSyn(const char* _buffer, int _size);
	bool receive(char* _buffer, int _size);
	bool getConnected();
	void reAccept();
private:
	struct command
	{
		const char* _buffer;
		int _size;
	};
	int d_packetSize;

	int d_packetHeadSize;
	int d_packetSizePos;
	int d_packetSizeLenth;
	std::function<void(char*, int _size)> d_dataHandlerCallBack;
	std::function<bool(SOCKET)> d_sendInitiliseCallBack;
	SOCKET	d_server;
	SOCKET	d_client;
	unsigned short d_serverPort;
	in_addr d_address;
	sockaddr_in d_sockaddr;
	TheQueue<command> d_sendQueue;
	TheQueue<command> d_receiveQueue;
	bool initialNetWorkForVariablePacketSize();
	void recvThreadPacketHead(std::promise<bool>& _promise);
	std::unique_ptr<std::promise<bool>> d_recvThreadPromisePtr;
	std::atomic<bool> d_recvDeadThreadRun;
	std::atomic<bool> d_sendDeadThreadRun;
	std::atomic<bool> d_connected;
	void sendThread(std::promise<bool>& _promise);
	std::unique_ptr<std::promise<bool>> d_sendThreadPromisePtr;
	bool d_readyToAccept;
};

