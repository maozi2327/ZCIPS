#include "ZCIPSMainFrame.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ZCIPSMainFrame w;
	w.show();
	return a.exec();
}
