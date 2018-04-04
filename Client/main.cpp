#include "client.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Client w; // GUI
	w.show(); // Show GUI
	return a.exec();
}
