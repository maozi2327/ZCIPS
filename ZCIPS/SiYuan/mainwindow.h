#pragma once

#include <QMainWindow>
#include "ui_mainwindow.h"
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

private:
	Ui::MainWindow ui;

private slots:
	void sizeChangedSlot(int _height);
};
