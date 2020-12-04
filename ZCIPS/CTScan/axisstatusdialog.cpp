#include "stdafx.h"
#include "axisstatusdialog.h"
#include "controllerinterface.h"

AxisStatusDialog::AxisStatusDialog(ControllerInterface* _controller, 
	const std::map<Axis, AxisData>& _axisDataMap, QWidget *parent)
	: QDialog(parent), d_controller(_controller)
{
	for (auto& itr : _axisDataMap)
	{
		auto controlsCombo = new AxisStatusControls(itr.second.axisCaption, 20,
			50, 50, 20, this);
		d_axisStatusControls.insert({ itr.first, controlsCombo });
	}
	
	d_gridLayout = new QGridLayout(this);
	auto title = new AxisStatusControls(QString::fromLocal8Bit("ÖáÃû³Æ"), 20, 50, 50, 20, this);
	int row = 0, column = 0;
	d_gridLayout->addWidget(title->d_name, row, column++);
	title->d_coordinate->setText(QString::fromLocal8Bit("×ø±ê")); 
	d_gridLayout->addWidget(title->d_coordinate, row, column++);
	title->d_ELp->setText(QString::fromLocal8Bit("ELp"));
	d_gridLayout->addWidget(title->d_ELp, row, column++);
	title->d_SDp->setText(QString::fromLocal8Bit("SDp"));
	d_gridLayout->addWidget(title->d_SDp, row, column++);
	title->d_SDn->setText(QString::fromLocal8Bit("SD-"));
	d_gridLayout->addWidget(title->d_SDn, row, column++);
	title->d_ELn->setText(QString::fromLocal8Bit("EL-"));
	d_gridLayout->addWidget(title->d_ELn, row, column++);
	title->d_zeroFound->setText(QString::fromLocal8Bit("Áãµã"));
	d_gridLayout->addWidget(title->d_zeroFound, row, column++);
	++row;

	for(auto& itr : d_axisStatusControls)
	{
		int column = 0;
		d_gridLayout->addWidget(itr.second->d_name, row, column++);
		d_gridLayout->addWidget(itr.second->d_coordinate, row, column++);
		d_gridLayout->addWidget(itr.second->d_ELp, row, column++);
		d_gridLayout->addWidget(itr.second->d_SDp, row, column++);
		d_gridLayout->addWidget(itr.second->d_SDn, row, column++);
		d_gridLayout->addWidget(itr.second->d_ELn, row, column++);
		++row;
	}
	
	ui.setupUi(this);
	d_timer = new QTimer(this);
	d_timer->start(200);
	connect(d_timer, &QTimer::timeout, this, &AxisStatusDialog::updateSlot);
}

AxisStatusDialog::~AxisStatusDialog()
{
}

void AxisStatusDialog::updateSlot()
{
	auto axisStatus = d_controller->readAxisStatus();

	for (auto& itr : axisStatus)
	{
		d_axisStatusControls[itr.first]->d_coordinate->
			setText(QString("1%").arg(itr.second.coordinate, 0, 'f', 2));
		QString zero(QString::fromLocal8Bit("0"));
		QString one(QString::fromLocal8Bit("1"));
		d_axisStatusControls[itr.first]->d_ELp->setText(itr.second.elp ? one : zero);
		d_axisStatusControls[itr.first]->d_SDp->setText(itr.second.sdp ? one : zero);
		d_axisStatusControls[itr.first]->d_SDn->setText(itr.second.sdn ? one : zero);
		d_axisStatusControls[itr.first]->d_ELn->setText(itr.second.eln ? one : zero);
		d_axisStatusControls[itr.first]->d_zeroFound->setText(itr.second.zeroFound ? one : zero);
	}
}