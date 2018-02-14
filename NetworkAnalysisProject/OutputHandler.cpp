#include "OutputHandler.h"
#include "global.h"

OutputHandler::OutputHandler(QObject *parent)
	: QObject(parent)
{


}

OutputHandler::~OutputHandler()
{
}

void OutputHandler::clientFileInit(std::string fileName, unsigned packetSize)
{
	inputFile.open(fileName, std::fstream::in | std::fstream::binary);
	inputFile.seekg(0, inputFile.end);
	fileSize = inputFile.tellg();
	inputFile.seekg(0, inputFile.beg);

}

void OutputHandler::clientPacketInit(unsigned numPackets, unsigned packetSize)
{
	this->numPackets = numPackets;
	this->packetSize = packetSize;
}

void OutputHandler::serverInit(std::string outputFile, unsigned maxPacketSize)
{
	this->outputFile.open(outputFile, std::fstream::out | std::fstream::binary);

}

//returns neg value when eof
int OutputHandler::getNextOutput(char * buffer, int* length)
{
	switch (mode)
	{
	case FILE_IN:
		if (!inputFile.eof())
		{
			std::lock_guard<std::mutex> guard(file_write_mutex);
			inputFile.read(buffer, packetSize);
			*length = packetSize;
			qDebug() << "failed to open file";
			return 1;
		}
		else
		{
			return -1;
		}
		
	case PACKET_IN:

		if (packetsSent < numPackets)
		{
			memset(buffer, 0x05, packetSize);
			*length = packetSize;
			return 1;
		}
		else
		{
			return -1;
		}
		
	}

	return 0;
}

int OutputHandler::writeToFile(char * buffer, int length)
{
	if (outputFile.is_open())
	{
		std::lock_guard<std::mutex> guard(file_write_mutex);
		outputFile.write(buffer, length);
		return 1;
	} 
	else
	{
		qDebug() << "failed to open file";
		return -1;
	}

}

int OutputHandler::getPacketSize()
{
	return packetSize;
}
