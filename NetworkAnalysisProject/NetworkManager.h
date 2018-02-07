#pragma once
#pragma comment(lib,"ws2_32.lib")

#include <QObject>
#include <QDataStream>
#include <QFile>
#include <QString>

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
#define PORT 9898


typedef struct CONNECTION {
	OVERLAPPED overlapped;
	SOCKET socket;
	CHAR Buffer[BUF_SIZE];
	DWORD sendBytes;
	DWORD rcvBytes;
} CONNECTION, *LP_CONNECTION;


class NetworkManager
{

public:
	NetworkManager();
	~NetworkManager();

private:
	void sendUdp(QByteArray data, int size);
	void sendTcp(QByteArray data, int size);
	DWORD WINAPI WorkerThread(LPVOID lpParameter);


	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
	WSAOVERLAPPED overlapped;
	SOCKET acceptSocket;
	SOCKET listenSocket;
	WSABUF buf;
	WSADATA wsaData;
	WSAEVENT AcceptEvent;
	HANDLE ThreadHandle;
	DWORD threadId;

	SOCKADDR_IN addr;


};
