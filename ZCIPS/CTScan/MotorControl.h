#pragma once

#include <QWidget>
#include "ui_MotorControl.h"
#include <map>

class ControllerInterface;
class MotorControl : public QWidget
{
	Q_OBJECT
private:
	ControllerInterface* d_controller;

public:
	MotorControl(ControllerInterface* in_controller, QWidget *parent = Q_NULLPTR);
	~MotorControl();


private:
	Ui::MotorControl ui;
	void updateAxisStatus();
private slots:
	void on_objRotAbsPosButton_clicked();
	void on_objRotNegativeButton_clicked();
	void on_objRotPositiveButton_clicked();

	void on_objTranslationAbsPosButton_clicked();
	void on_objTranslationNegativeButton_clicked();
	void on_objTranslationPositiveButton_clicked();

	void on_objRadialAbsPosButton_clicked();
	void on_objRadialNegativeButton_clicked();
	void on_objRadialPositiveButton_clicked();

	void on_detLayerAbsPosButton_clicked();
	void on_detLayerNegativeButton_clicked();
	void on_detLayerPositiveButton_clicked();

	void on_detTranslationAbsPosButton_clicked();
	void on_detTranslationNegativeButton_clicked();
	void on_detTranslationPositiveButton_clicked();

	void on_detRadialAbsPosButton_clicked();
	void on_detRadialNegativeButton_clicked();
	void on_detRadialPositiveButton_clicked();

	void on_rayLayerAbsPosButton_clicked();
	void on_rayLayerNegativeButton_clicked();
	void on_rayLayerPositiveButton_clicked();
};
