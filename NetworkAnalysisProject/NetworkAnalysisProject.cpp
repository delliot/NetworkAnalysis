#include "NetworkAnalysisProject.h"

NetworkAnalysisProject::NetworkAnalysisProject(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
			

	QLabel* modeLabel = new QLabel("Mode: ");
	ui.statusBar->addWidget(modeLabel);

	OutputHandler * handler = new OutputHandler(this);
	handler->clientPacketInit(10, 1024);

	NetworkManager* net = new NetworkManager(5555, "127.0.0.1", 1024, handler);

	net->setProtocol(TCP);
	net->startClient();

	
}

void NetworkAnalysisProject::addSentPackets(unsigned int packets)
{
	numPacketsSent += packets;
	ui.lineEdit_packet_sent->setText(QString(numPacketsSent));
}

void NetworkAnalysisProject::addRecvPackets(unsigned int packets)
{
	numPacketsRecv += packets;
	ui.lineEdit_packet_received->setText(QString(numPacketsRecv));
}

void NetworkAnalysisProject::addBytesTransferred(unsigned int bytes) 
{
	numBytesTransferred += bytes;
	ui.lineEdit_data_transferred->setText(QString(numBytesTransferred));
}