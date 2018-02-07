#pragma once
#pragma comment(lib,"ws2_32.lib")

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
#define PORT 9898
#define MAX_CONNECTIONS


typedef struct CONNECTION {
	OVERLAPPED overlapped;
	SOCKET socket;
	CHAR buffer[BUF_SIZE];
	WSABUF wsaBuf;
	DWORD sendBytes;
	DWORD rcvBytes;
} CONNECTION, *LP_CONNECTION;


class NetworkManager
{

public:
	NetworkManager();
	int init();
	~NetworkManager();

private:
	void sendUdp(QByteArray data, int size);
	void sendTcp(QByteArray data, int size);
	
	void CALLBACK ioRoutine(DWORD Error, DWORD BytesTransferred,
		LPWSAOVERLAPPED Overlapped, DWORD InFlags);

	DWORD WINAPI ioThread();

	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
	WSAOVERLAPPED overlapped;
	SOCKET acceptSocket;
	SOCKET listenSocket;
	WSABUF buf;
	WSADATA wsaData;
	WSAEVENT AcceptEvent;
	HANDLE ThreadHandle;
	DWORD threadId;
	HANDLE WINAPI iop;

	int threadCount;
	bool running;

	
	SOCKADDR_IN addr;

	class IoThread : public QRunnable
	{
	public:
		NetworkManager * self;

		IoThread(NetworkManager* instance)
			:self(instance)
		{

		}

		void run()
		{
			//handle completion port 
			while (self->running)
			{
				//GetQueuedCompletionStatus
			}
		}
	};


};
