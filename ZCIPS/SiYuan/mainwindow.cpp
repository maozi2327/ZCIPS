#include "stdafx.h"
#include "mainwindow.h"
#include "../CTScan/ctscanexport.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	auto ctScan = CTScanInterface::loadCtScanInstance(ui.centralWidget);
	auto menuBar = ctScan->getMenuBar();
	//menuBar->setParent(this);
	setMenuBar(menuBar);
	connect(ctScan, &CTScanInterface::sizeChangedSignal, this, &MainWindow::sizeChangedSlot);
}

MainWindow::~MainWindow()
{
}

void MainWindow::sizeChangedSlot(int _height)
{
	setFixedHeight(_height + ui.statusBar->height() + 2);
	updateGeometry();
}