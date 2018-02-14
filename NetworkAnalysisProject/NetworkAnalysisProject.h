#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_NetworkAnalysisProject.h"
#include "OutputHandler.h"
#include "NetworkManager.h"
#include "global.h"




class NetworkAnalysisProject : public QMainWindow
{
	Q_OBJECT

public:
	NetworkAnalysisProject(QWidget *parent = Q_NULLPTR);


public slots:
	void addSentPackets(unsigned int packets);
	void addRecvPackets(unsigned int packets);
	void addBytesTransferred(unsigned int bytes);

private:
	Ui::NetworkAnalysisProject ui;

	void setModeLabel(CONNECT_MODE m);

	unsigned int numPacketsRecv;
	unsigned int numPacketsSent;
	unsigned int numBytesTransferred;
	
};
