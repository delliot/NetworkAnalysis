#include "NetworkManager.h"
#include "global.h"

NetworkManager::NetworkManager(int port, std::string ipAddr, unsigned packetSize, OutputHandler * outputHandler)
	:running(true)
	,port(port)
	,ipAddr(ipAddr)
	,clientSocket(INVALID_SOCKET)
	,packetSize(packetSize)
	,outputHandler(outputHandler)
{
	threadCount = 4;
}

int NetworkManager::startServer()
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

	HANDLE check;
	sockaddr_in client_address;
	int address_size = sizeof(client_address);

	switch (mode)
	{
	case TCP:

		if ((listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
			WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
		{
			printf("Failed to get a socket %d\n", WSAGetLastError());
			return -1;
		}

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port);

		if (bind(listenSocket, (PSOCKADDR)&addr,
			sizeof(addr)) == SOCKET_ERROR)
		{
			printf("bind() failed with error %d\n", WSAGetLastError());
			return -1;
		}

		if (listen(listenSocket, 10) == SOCKET_ERROR)
		{
			printf("listen() failed with error %d\n", WSAGetLastError());
			return -1;
		}
		

		clientSocket = ::WSAAccept(
			listenSocket,
			reinterpret_cast<sockaddr*>(&client_address),
			&address_size, 0, 0);
		
		if (clientSocket == INVALID_SOCKET) {
			int ret = WSAGetLastError();
			printf("WSAAccept failed, closing listening socket %u: error: %d\n", (unsigned int)listenSocket, ret);
			return -1;
		}

		check = CreateIoCompletionPort((HANDLE)clientSocket, iop, (DWORD)clientSocket, 0);
		if (check == NULL)
		{
			printf("Error occurred executing CreateIoCompletionPort()\n");
		}

		break;
	case UDP:
	{
		if ((listenSocket = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0,
			WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
		{
			printf("Failed to get a socket %d\n", WSAGetLastError());
			return -1;
		}

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port);

		if (bind(listenSocket, (PSOCKADDR)&addr,
			sizeof(addr)) == SOCKET_ERROR)
		{
			printf("bind() failed with error %d\n", WSAGetLastError());
			return -1;
		}


		check = CreateIoCompletionPort((HANDLE)listenSocket, iop, (DWORD)listenSocket, 0);
		if (check == NULL)
		{
			printf("Error occurred executing CreateIoCompletionPort()\n");
		}
		

		break;
	}
	}
		
	


	return 1;
}

int NetworkManager::startClient()
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


	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_protocol = IPPROTO_TCP;
	switch (mode)
	{
	case UDP:
		hints.ai_socktype = SOCK_DGRAM;
		break;
	case TCP:
		hints.ai_socktype = SOCK_STREAM;
		break;
	}

	

	std::string * portStr = new std::string("" + port);
	
	// Resolve the server address and port
	int iResult = getaddrinfo(ipAddr.c_str(), portStr->c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;

	ptr = result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
		ptr->ai_protocol);


	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		int r = WSAGetLastError();
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}



	return 0;
}

void NetworkManager::setProtocol(CONNECT_MODE m)
{
	mode = m;
}


int NetworkManager::sendAll(SOCKET socket)
{
	int bufsize = outputHandler->getPacketSize();
	int retVal = 0;
	DWORD bytes_sent;
	DWORD flags = 0;
	for (;;)
	{
		ioOperation* op = new ioOperation(this, bufsize);
		op->socket = socket;
		op->optype = OP_WRITE;

		int lengthToWrite;
		retVal = outputHandler->getNextOutput(op->buffer, &lengthToWrite);

		if (retVal < 0)
		{
			break;
		}
		
		int send_retVal;
		send_retVal = WSASend(socket, &op->wsabuf, 1, &bytes_sent, flags, (OVERLAPPED *)op, NULL);

	}

	return 1;
}

int NetworkManager::recv_data(SOCKET client)
{
	int bufsize = outputHandler->getPacketSize();
	ioOperation * op = new ioOperation(this, bufsize);
	op->socket = client;
	op->optype = OP_READ;
	DWORD flags = 0;
	DWORD bytes_received;

	int retval = WSARecv(client,
		&op->wsabuf, 1, &bytes_received, &flags, (OVERLAPPED*)op, NULL);

	return 0;
}

int NetworkManager::read_to_file(char * data, int length)
{
	outputHandler->writeToFile(data, length);

	return 0;
}

NetworkManager::IoThread::IoThread(NetworkManager* instance)
	:self(instance)
{
	iocp = self->iop;
}

void NetworkManager::IoThread::run()
{
	int bufsize = self->outputHandler->getPacketSize();
	DWORD bytes_transferred;
	ULONG_PTR key;
	
	ioOperation * o = new ioOperation(self, bufsize);
	// handle completion port
		while (true)
		{
			if (GetQueuedCompletionStatus(iocp, &bytes_transferred, &key, (OVERLAPPED **)&o, 100) == 0)
			{
				if (o == 0)
				{
					//overlapped is null
				}
				else
				{
					free(o);
				}

				continue;
			}

			if (bytes_transferred == 0 && key == 0 && o == 0)
			{
				PostQueuedCompletionStatus(iocp, 0, 0, 0);
				break;
			}
			else 
			{
				switch (o->optype)
				{
					case OP_READ:
						//read from buffer

						self->read_to_file(o->buffer, bytes_transferred);
						self->recv_data((SOCKET) key);
						break;
					case OP_WRITE:
						o->remaining_bytes -= bytes_transferred;

						if (o->remaining_bytes == 0)
						{
							//finsished sending 
						}
						break;
				}
			}
		}
}


NetworkManager::ioOperation::ioOperation(NetworkManager * const parent, long bufsize)
	:num_bytes_to_send(0)
	,remaining_bytes(0)
	,socket(INVALID_SOCKET)
{

	Internal = InternalHigh = 0;
	Offset = OffsetHigh = 0;
	hEvent = 0;

	_parent = parent;
	buffer = new char[bufsize + 1]();

	wsabuf.len = bufsize + 1;
	wsabuf.buf = buffer;
}

NetworkManager::ioOperation::~ioOperation()
{
	delete[] buffer;
}


NetworkManager::~NetworkManager()
{
}


