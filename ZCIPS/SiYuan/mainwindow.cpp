#include "stdafx.h"
#include "mainwindow.h"
#include "../CTScan/ctscanexport.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	auto ctScan = CTScanInterface::loadCtScanInstance(ui.widget);
	auto menuBar = ctScan->getMenuBar();
	//menuBar->setParent(this);
	setMenuBar(menuBar);
}

MainWindow::~MainWindow()
{
}
