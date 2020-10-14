#include "stdafx.h"
#include "siyuan.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SiYuan w;
	w.show();
	return a.exec();
}
