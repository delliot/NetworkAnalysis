#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
	int returnval;
	if (returnval = (WSAStartup(0x0202, &wsaData)) != 0)
	{
		printf("WSAStartup failed with error %d\n", returnval);
		WSACleanup();
		return;
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

	if (listen(listenSocket, 2))
	{
		printf("listen() failed with error %d\n", WSAGetLastError());
		return;
	}

	if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
		return;
	}


	if ((ThreadHandle = CreateThread(NULL, 0, ioThread, (LPVOID)AcceptEvent, 0, &ThreadId)) == NULL)
	{
		printf("CreateThread failed with error %d\n", GetLastError());
		return;
	}
}

NetworkManager::~NetworkManager()
{
}
