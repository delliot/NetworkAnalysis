#pragma once
#pragma comment(lib,"ws2_32.lib")

#include <NetworkAnalysisProject.h>
#include "global.h"

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include <QFile>
#include <QString>
#include <QRunnable>
#include <QThreadPool>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>

#define	PACKET_SIZE_1K	1024
#define	PACKET_SIZE_4K	4096
#define PACKET_SIZE_20K 20000
#define PACKET_SIZE_60K 60000
#define PACKET_SIZE_80K 80000

#define USE_TCP		1
#define USE_UDP	   -1

#define BUF_SIZE 8192
#define MAX_CONNECTIONS 10



class NetworkManager
{

public:
	NetworkManager(int port, std::string ipAddr, unsigned packetSize, OutputHandler * outputHandler);
	int startServer();
	int startClient();
	int sendAll(SOCKET socket);
	void setProtocol(CONNECT_MODE m);
	~NetworkManager();

	class IoThread : public QRunnable
	{
	public:
		NetworkManager * self;
		IoThread(NetworkManager * self);

		void run();

	private:
		HANDLE WINAPI iocp;
	};


	enum OP_TYPE {OP_READ, OP_WRITE};

	class ioOperation : public OVERLAPPED
	{
	public:
		WSABUF wsabuf;
		SOCKET socket;
		NetworkManager * _parent;
		OP_TYPE optype;
		long num_bytes_to_send;
		long remaining_bytes;
		char * buffer;

		ioOperation(NetworkManager * const parent, long bufsize);
		~ioOperation();
	};

private:
	int port;
	std::string ipAddr;
	int recv_data(SOCKET client);
	int read_to_file(char* data, int length);
	unsigned packetSize;

	OutputHandler * outputHandler;

	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
	WSAOVERLAPPED overlapped;
	SOCKET clientSocket;
	SOCKET listenSocket;
	WSABUF buf;
	WSADATA wsaData;
	WSAEVENT AcceptEvent;
	HANDLE ThreadHandle;
	DWORD threadId;
	HANDLE WINAPI iop;
	struct hostent * host;

	int threadCount;
	bool running;
	CONNECT_MODE mode;
	SOCKADDR_IN addr;
};
