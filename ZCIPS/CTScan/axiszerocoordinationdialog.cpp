#include "stdafx.h"
#include "axiszerocoordinationdialog.h"
#include "controllerinterface.h"

AxisZeroCoordinationDialog::AxisZeroCoordinationDialog(ControllerInterface* _controller, 
	const std::map<Axis, AxisData>& _axisDataMap, QWidget *parent)
	: QDialog(parent), d_controller(_controller)
{
	for (auto& itr : _axisDataMap)
	{
		auto controlsCombo = new AxisControl(itr.second.axisCaption, itr.second.axisName, this);
		d_axisControls.insert({ itr.first, controlsCombo });
	}

	d_gridLayout = new QGridLayout(this);
	int i = 0;

	for (auto& itr : d_axisControls)
	{
		int row = i / 2 * 2;
		int column = i % 2;
		d_gridLayout->addWidget(itr.second->axisNameLabel, row, column);
		d_gridLayout->addWidget(itr.second->axisZeroCoordinationEdit, row + 1, column);
		++i;
	}

	d_uploadButton = new QPushButton(this);
	d_uploadButton->setObjectName(QStringLiteral("uploadButton"));
	d_uploadButton->setText(QString::fromLocal8Bit("ÉÏ´«"));
	d_downloadButton = new QPushButton(this);
	d_downloadButton->setObjectName(QStringLiteral("downloadButton"));
	d_downloadButton->setText(QString::fromLocal8Bit("ÏÂÔØ"));
	i = (i + 1) / 2 * 2;
	d_gridLayout->addWidget(d_uploadButton, i / 2 * 2, 0);
	d_gridLayout->addWidget(d_downloadButton, i / 2 * 2, 1);

	ui.setupUi(this);
}

AxisZeroCoordinationDialog::~AxisZeroCoordinationDialog()
{

}

void AxisZeroCoordinationDialog::on_uploadButton_clicked()
{
	std::map<Axis, float> axisZero = d_controller->readAxisWorkZero();

	for (auto& itr : axisZero)
		d_axisControls[itr.first]->axisZeroCoordinationEdit->
			setText(QString("%1").arg(itr.second, 0, 'f', 2));
	//for(auto& axisZeroItr : axisZero)
	//	for (auto& AxisNameZeroMapItr : AxisNameZeroMap)
	//	{
	//		if (axisZeroItr.first == AxisNameZeroMapItr.second)
	//		{
	//			auto controlItr = std::find_if(d_axisControls.begin(), d_axisControls.end(),
	//				[=](AxisControl* axisControl) { return AxisNameZeroMapItr.first == axisControl->axis; });

	//			if (controlItr != d_axisControls.end())
	//				(*controlItr)->axisZeroCoordinationEdit->
	//					setText(QString("%1").arg(axisZeroItr.second, 0, 'f', 2));
	//		}
	//	}
}

void AxisZeroCoordinationDialog::on_downloadButton_clicked()
{
	std::map<Axis, float> axisZero;

	for (auto& itr : d_axisControls)
	{
		float value = itr.second->axisZeroCoordinationEdit->text().toFloat();
		auto axis = itr.first;
		axisZero.insert({ axis, value });
	}
	
	d_controller->setAxisWorkZero(axisZero);
}
