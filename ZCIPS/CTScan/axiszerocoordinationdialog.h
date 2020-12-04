#pragma once

#include <QDialog>
#include "ui_axiszerocoordinationdialog.h"
#include "../Public/headers/machineType.h"

class ControllerInterface;
class AxisZeroCoordinationDialog : public QDialog
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
		QLabel*	axisNameLabel;
		QLineEdit* axisZeroCoordinationEdit;

		AxisControl(const QString& _axisCaption, const QString& _objectName, QWidget* _parent)
		{
			axisNameLabel = new QLabel(_parent);
			axisNameLabel->setObjectName(_objectName + QString::fromLocal8Bit("Label"));
			axisNameLabel->setText(_axisCaption);
			axisZeroCoordinationEdit = new QLineEdit(_parent);
			axisZeroCoordinationEdit->setObjectName(_objectName + QString::fromLocal8Bit("LineEdit"));
		}
	};

	std::map<Axis, AxisControl*> d_axisControls;
	QGridLayout* d_gridLayout;
	QPushButton* d_uploadButton;
	QPushButton* d_downloadButton;
	ControllerInterface* d_controller;

private slots:
	void on_uploadButton_clicked();
	void on_downloadButton_clicked();
};
