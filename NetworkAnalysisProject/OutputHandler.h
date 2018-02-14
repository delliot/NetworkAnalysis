#pragma once

#include <QObject>
#include <fstream>
#include <QDebug>
#include <mutex>

#define FILE_IN	1
#define PACKET_IN	2

class OutputHandler : public QObject
{
	Q_OBJECT

public:
	OutputHandler(QObject *parent);
	~OutputHandler();

	void clientFileInit(std::string fileName, unsigned packetSize);
	void clientPacketInit(unsigned numPackets, unsigned packetSize);

	void serverInit(std::string outputFile, unsigned maxPacketSize);

	int getNextOutput(char * buffer, int* length);
	int writeToFile(char * buffer, int length);

	int getPacketSize();


	

	
private:
	// either 
	int mode;

	int packetSize;

	int numPackets;
	
	std::mutex file_write_mutex;
	std::string fileName;
	std::ifstream inputFile;
	std::ofstream outputFile;
	long fileSize;
	long curPos;
	long packetsSent;

};
