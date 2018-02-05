#include "NetworkAnalysisProject.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	NetworkAnalysisProject w;
	w.show();
	return a.exec();
}
