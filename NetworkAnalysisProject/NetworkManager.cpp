#include "NetworkManager.h"

NetworkManager::NetworkManager()
	:running(true)
{
	
}

int NetworkManager::init()
{
	int returnval;
	if (returnval = (WSAStartup(0x0202, &wsaData)) != 0)
	{
		printf("WSAStartup failed with error %d\n", returnval);
		WSACleanup();
		return -1;
	}

	iop = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		0,
		0,
		threadCount / 2);

	if ((iop) == 0)
	{
		qDebug() << "io completion port failed";
		return -1;
	}


	for (int i = 0; i < threadCount; i++)
	{
		//todo change thread constructor
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		IoThread* t = new IoThread(this);
		QThreadPool::globalInstance()->start(t);
	}

	if ((listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
		WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("Failed to get a socket %d\n", WSAGetLastError());
		return;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);

	if (bind(listenSocket, (PSOCKADDR)&addr,
		sizeof(addr)) == SOCKET_ERROR)
	{
		printf("bind() failed with error %d\n", WSAGetLastError());
		return;
	}
	
	if (listen(listenSocket, MAX_CONNECTIONS))
	{
		printf("listen() failed with error %d\n", WSAGetLastError());
		return;
	}

	if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
		return;
	}



}

DWORD WINAPI NetworkManager::ioThread()
{
	
}



NetworkManager::~NetworkManager()
{
}


