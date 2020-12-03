#include "stdafx.h"
#include "axiszerocoordinationdialog.h"
#include "controllerinterface.h"

AxisZeroCoordinationDialog::AxisZeroCoordinationDialog(ControllerInterface* _controller, 
	const std::map<Axis, AxisData>& _axisDataMap, QWidget *parent)
	: QWidget(parent), d_controller(_controller)
{
	for (auto& itr : _axisDataMap)
	{
		auto controlsCombo = new AxisControl(itr.first, itr.second.axisCaption, itr.second.axisName, this);
		d_axisControls.push_back(controlsCombo);
	}

	d_verticalLayout = new QVBoxLayout(this);

	for (auto& itr : d_axisControls)
	{
		d_verticalLayout->addWidget(itr->axisNameLabel);
		d_verticalLayout->addWidget(itr->axisZeroCoordinationEdit);
	}

	d_buttonHorizontalLayout = new QHBoxLayout();
	d_buttonHorizontalLayout->setSpacing(6);
	d_buttonHorizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
	d_uploadButton = new QPushButton(this);
	d_uploadButton->setObjectName(QStringLiteral("uploadButton"));
	d_uploadButton->setText(QString::fromLocal8Bit("ÉÏ´«"));
	d_buttonHorizontalLayout->addWidget(d_uploadButton);
	d_downloadButton = new QPushButton(this);
	d_downloadButton->setObjectName(QStringLiteral("downloadButton"));
	d_downloadButton->setText(QString::fromLocal8Bit("ÏÂÔØ"));
	d_buttonHorizontalLayout->addWidget(d_downloadButton);

	d_verticalLayout->addLayout(d_buttonHorizontalLayout);

	setFixedWidth(150);

	ui.setupUi(this);
}

AxisZeroCoordinationDialog::~AxisZeroCoordinationDialog()
{

}

void AxisZeroCoordinationDialog::on_uploadButton_clicked()
{
	std::map<AxisZeroEnum, float> axisZero = d_controller->readAxisWorkZero();

	for(auto& axisZeroItr : axisZero)
		for (auto& AxisNameZeroMapItr : AxisNameZeroMap)
		{
			if (axisZeroItr.first == AxisNameZeroMapItr.second)
			{
				auto controlItr = std::find_if(d_axisControls.begin(), d_axisControls.end(),
					[=](AxisControl* axisControl) { return AxisNameZeroMapItr.first == axisControl->axis; });

				if (controlItr != d_axisControls.end())
					(*controlItr)->axisZeroCoordinationEdit->
						setText(QString("%1").arg(axisZeroItr.second, 0, 'f', 2));
			}
		}
}

void AxisZeroCoordinationDialog::on_downloadButton_clicked()
{
	std::map<AxisZeroEnum, float> axisZero;

	for (auto& itr : d_axisControls)
	{
		float value = itr->axisZeroCoordinationEdit->text().toFloat();
		auto axis = itr->axis;

		auto axisZeroMapItr = AxisNameZeroMap.find(axis);

		if (axisZeroMapItr != AxisNameZeroMap.end())
			axisZero.insert({ axisZeroMapItr->second, value });
	}
	
	d_controller->setAxisWorkZero(axisZero);
}
