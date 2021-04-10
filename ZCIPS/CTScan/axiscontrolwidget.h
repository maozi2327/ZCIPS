#pragma once

#include <QWidget>
#include "ui_axiscontrolwidget.h"
#include "../Public/headers/machineType.h"
class ControllerInterface;

class AxisControlwidget : public QWidget
{
	Q_OBJECT

public:
	AxisControlwidget(ControllerInterface* _controller, const std::map<Axis, AxisData>& _axisDataMap, QWidget *parent = Q_NULLPTR);
	~AxisControlwidget();

private:
	Ui::AxisControlwidget ui;
	
	struct AxisControl
	{
		Axis axis;
		QRadioButton* radioButton;
		QLabel* axisPosLabel;

		AxisControl(Axis _axis, const QString& _axisCaption, const QString& _objectName, QWidget* _parent)
		{
			axis = _axis;
			radioButton = new QRadioButton(_parent);
			radioButton->setObjectName(_objectName + QString::fromLocal8Bit("RadiaoButton"));
			radioButton->setText(_axisCaption);
			axisPosLabel = new QLabel(_parent);
			axisPosLabel->setObjectName(_objectName + QString::fromLocal8Bit("Label"));
		}
	};
	
	std::vector<AxisControl*> d_axisControls;	
	QPushButton* d_negativePosButton;
	QPushButton* d_positivePosButton;
	QPushButton* d_absPosButton;
	QPushButton* d_stopButton;
	QLineEdit* d_posLineEdit;

	QGridLayout* d_axisGridLayout;
	QGridLayout* d_posAreaGridLayout;
	QSpacerItem* d_horizontalSpacer;
	QHBoxLayout* d_horizontalLayout;

	ControllerInterface* d_controller;
	void updateStatus();
	void updateAxisStatus();
	void axisRelMove(Axis _axis, float pos);
	QTimer* d_timer;
	QButtonGroup* d_axisRadioButtonGroup;
	Axis d_axisSelected;

private slots:
	void on_negativePosButton_clicked();
	void on_positivePosButton_clicked();
	void on_stopButton_clicked();
	void on_absPosButton_clicked();
	void onRadionButtonClicked(QAbstractButton * d_button);
};
