#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_NetworkAnalysisProject.h"

class NetworkAnalysisProject : public QMainWindow
{
	Q_OBJECT

public:
	NetworkAnalysisProject(QWidget *parent = Q_NULLPTR);

private:
	Ui::NetworkAnalysisProjectClass ui;
};
