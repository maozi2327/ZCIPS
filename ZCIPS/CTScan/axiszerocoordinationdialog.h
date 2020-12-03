#pragma once

#include <QWidget>
#include "ui_axiszerocoordinationdialog.h"
#include "../Public/headers/machineType.h"
#include "controllerinterface.h"

class AxisZeroCoordinationDialog : public QWidget
{
	Q_OBJECT

public:
	AxisZeroCoordinationDialog(ControllerInterface* _controller, const std::map<Axis, AxisData>& _axisDataMap, 
		QWidget *parent = Q_NULLPTR);
	~AxisZeroCoordinationDialog();

private:
	Ui::AxisZeroCoordinationDialog ui;

	struct AxisControl
	{
		Axis axis;
		QLabel*	axisNameLabel;
		QLineEdit* axisZeroCoordinationEdit;

		AxisControl(Axis _axis, const QString& _axisCaption, const QString& _objectName, QWidget* _parent)
		{
			axis = _axis;
			axisNameLabel = new QLabel(_parent);
			axisNameLabel->setObjectName(_objectName + QString::fromLocal8Bit("Label"));
			axisNameLabel->setText(_axisCaption);
			axisZeroCoordinationEdit = new QLineEdit(_parent);
			axisZeroCoordinationEdit->setObjectName(_objectName + QString::fromLocal8Bit("LineEdit"));
		}
	};

	std::vector<AxisControl*> d_axisControls;
	QVBoxLayout* d_verticalLayout;
	QHBoxLayout* d_buttonHorizontalLayout;
	QPushButton* d_uploadButton;
	QPushButton* d_downloadButton;
	ControllerInterface* d_controller;

private slots:
	void on_uploadButton_clicked();
	void on_downloadButton_clicked();
};
