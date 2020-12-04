#pragma once

#include <QDialog>
#include "ui_axisstatusdialog.h"
#include "../Public/headers/machineType.h"
#include <vector>

class ControllerInterface; 
class AxisStatusDialog : public QDialog
{
	Q_OBJECT

public:
	AxisStatusDialog(ControllerInterface* _controller, const std::map<Axis, AxisData>& _axisDataMap, 
		QWidget *parent = Q_NULLPTR);
	~AxisStatusDialog();

private:
	Ui::AxisStatusDialog ui;
	ControllerInterface* d_controller;

	struct AxisStatusControls
	{
		QLabel* d_name;
		QLabel* d_coordinate;
		QLabel* d_ELp;
		QLabel* d_ELn;
		QLabel* d_SDn;
		QLabel* d_SDp;
		QLabel* d_zeroFound;

		AxisStatusControls(const QString& _name, int _height, int _nameWidth,
			int _coordinateWidth, int _width, QWidget* _parent)
			: d_name(new QLabel(_parent)), d_coordinate(new QLabel(_parent))
			, d_ELp(new QLabel(_parent)), d_ELn(new QLabel(_parent))
			, d_SDp(new QLabel(_parent)), d_SDn(new QLabel(_parent))
			, d_zeroFound(new QLabel(_parent))
		{
			d_name->setFixedWidth(_nameWidth);
			d_name->setText(_name);
			d_coordinate->setFixedWidth(_coordinateWidth);
			d_ELp->setFixedWidth(_width);
			d_ELp->setAlignment(Qt::AlignCenter);
			d_ELn->setFixedWidth(_width);
			d_ELn->setAlignment(Qt::AlignCenter);
			d_SDn->setFixedWidth(_width);
			d_SDn->setAlignment(Qt::AlignCenter);
			d_SDp->setFixedWidth(_width);
			d_SDp->setAlignment(Qt::AlignCenter);
			d_zeroFound->setFixedWidth(_width);
			d_zeroFound->setAlignment(Qt::AlignCenter);
		}

	};

	std::map<Axis, AxisStatusControls*> d_axisStatusControls;
	QGridLayout* d_gridLayout;
	QTimer* d_timer;

private slots:
	void updateSlot();
};
