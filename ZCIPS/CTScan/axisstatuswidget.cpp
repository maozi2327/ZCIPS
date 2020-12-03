#include "stdafx.h"
#include "axisstatuswidget.h"
#include "controllerinterface.h"

AxisStatusWidget::AxisStatusWidget(ControllerInterface* _controller, const std::map<Axis, AxisData>& _axisDataMap, QWidget *parent)
	: QWidget(parent), d_controller(_controller)
{
	for (auto& itr : _axisDataMap)
	{
		auto controlsCombo = new AxisControl(itr.first, itr.second.axisCaption, itr.second.axisName, this);
		controlsCombo->radioButton->setMinimumSize(QSize(13, 0));
		controlsCombo->radioButton->setMaximumSize(QSize(13, 16777215));

		controlsCombo->axisNameLabel->setMinimumSize(QSize(60, 0));
		controlsCombo->axisNameLabel->setMaximumSize(QSize(60, 16777215));
		controlsCombo->axisNameLabel->setFrameShape(QFrame::Box);

		controlsCombo->axisPosLabel->setMinimumSize(QSize(60, 0));
		controlsCombo->axisPosLabel->setMaximumSize(QSize(60, 16777215));
		controlsCombo->axisPosLabel->setFrameShape(QFrame::Box);
		d_axisControls.push_back(controlsCombo);
	}

	d_horizontalLayout = new QHBoxLayout(this);
	d_horizontalLayout->setSpacing(6);
	d_horizontalLayout->setContentsMargins(11, 11, 11, 11);
	d_horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

	d_axisGridLayout = new QGridLayout();
	d_axisGridLayout->setSpacing(6);
	d_axisGridLayout->setObjectName(QStringLiteral("axisGridLayout"));

	for (int i = 0; i != d_axisControls.size(); ++i)
	{
		int pos = 0;
		d_axisGridLayout->addWidget(d_axisControls[i]->radioButton, i, pos++, 1, 1);
		d_axisGridLayout->addWidget(d_axisControls[i]->axisNameLabel, i, pos++, 1, 1);
		d_axisGridLayout->addWidget(d_axisControls[i]->axisPosLabel, i, pos++, 1, 1);
	}

	d_horizontalLayout->addLayout(d_axisGridLayout);

	d_horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	d_horizontalLayout->addItem(d_horizontalSpacer);


	//右侧坐标输入、正反、绝对和停止按钮的布局
	d_posAreaGridLayout = new QGridLayout();
	d_posAreaGridLayout->setSpacing(6);
	d_posAreaGridLayout->setObjectName(QStringLiteral("gridLayout"));
	d_posLineEdit = new QLineEdit(this);
	d_posLineEdit->setObjectName(QStringLiteral("posLineEdit"));
	d_posLineEdit->setMinimumSize(QSize(60, 0));
	d_posLineEdit->setMaximumSize(QSize(60, 16777215));

	d_posAreaGridLayout->addWidget(d_posLineEdit, 0, 0, 1, 2);

	d_negativePosButton = new QPushButton(this);
	d_negativePosButton->setObjectName(QStringLiteral("negativePosButton"));
	d_negativePosButton->setText(QString::fromLocal8Bit("反向"));
	d_negativePosButton->setMinimumSize(QSize(25, 25));
	d_negativePosButton->setMaximumSize(QSize(25, 25));

	d_posAreaGridLayout->addWidget(d_negativePosButton, 1, 0, 1, 1);

	d_positivePosButton = new QPushButton(this);
	d_positivePosButton->setObjectName(QStringLiteral("positivePosButton"));
	d_positivePosButton->setText(QString::fromLocal8Bit("正向"));
	d_positivePosButton->setMinimumSize(QSize(25, 25));
	d_positivePosButton->setMaximumSize(QSize(25, 25));

	d_posAreaGridLayout->addWidget(d_positivePosButton, 1, 1, 1, 1);

	d_absPosButton = new QPushButton(this);
	d_absPosButton->setObjectName(QStringLiteral("absPosButton"));
	d_absPosButton->setText(QString::fromLocal8Bit("绝对定位"));
	d_absPosButton->setMinimumSize(QSize(30, 30));
	d_absPosButton->setMaximumSize(QSize(70, 30));

	d_posAreaGridLayout->addWidget(d_absPosButton, 2, 0, 1, 2);

	d_stopButton = new QPushButton(this);
	d_stopButton->setObjectName(QStringLiteral("stopButton"));
	d_stopButton->setText(QString::fromLocal8Bit("停止"));
	QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(d_stopButton->sizePolicy().hasHeightForWidth());
	d_stopButton->setSizePolicy(sizePolicy1);
	d_stopButton->setMinimumSize(QSize(50, 50));
	d_stopButton->setMaximumSize(QSize(100, 100));

	d_posAreaGridLayout->addWidget(d_stopButton, 3, 0, 1, 2);

	d_horizontalLayout->addLayout(d_posAreaGridLayout);

	d_axisRadioButtonGroup = new QButtonGroup(this);
	d_axisRadioButtonGroup->setExclusive(true);

	d_axisControls[0]->radioButton->setChecked(true);
	d_axisSelected = d_axisControls[0]->axis;

	ui.setupUi(this);

	for (auto& itr : d_axisControls)
		d_axisRadioButtonGroup->addButton(itr->radioButton);

	connect(d_axisRadioButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
		this, SLOT(onRadionButtonClicked(QAbstractButton*)));
	d_timer = new QTimer(this);
	d_timer->start(100);
	connect(d_timer, &QTimer::timeout, this, &AxisStatusWidget::updateStatus);
}

AxisStatusWidget::~AxisStatusWidget()
{
}

void AxisStatusWidget::on_negativePosButton_clicked()
{
	d_controller->axisRelMove(d_axisSelected, -d_posLineEdit->text().toFloat());
}
void AxisStatusWidget::updateStatus()
{
	std::map<AxisPosEnum, float> axisPos = d_controller->readAxisPostion();

	for (auto& itr : d_axisControls)
	{
		auto axisPosEnumItr = AxisNamePosMap.find(itr->axis);

		if(axisPosEnumItr != AxisNamePosMap.end())
			itr->axisPosLabel->setText(QString("%1").arg(axisPos[axisPosEnumItr->second], 0, 'f', 2));
	}
}
void AxisStatusWidget::updateAxisStatus()
{
}
void AxisStatusWidget::on_positivePosButton_clicked()
{
	d_controller->axisRelMove(d_axisSelected, d_posLineEdit->text().toFloat());
}
void AxisStatusWidget::on_stopButton_clicked()
{
	d_controller->stopAll();
}
void AxisStatusWidget::on_absPosButton_clicked()
{
	d_controller->axisAbsMove(d_axisSelected, d_posLineEdit->text().toFloat());
}
void AxisStatusWidget::onRadionButtonClicked(QAbstractButton * _button)
{
	auto itr = std::find_if(d_axisControls.begin(), d_axisControls.end(),
		[=](AxisControl* _ptr) { return _ptr->radioButton == _button; });
	
	if (itr != d_axisControls.end())
		d_axisSelected = (*itr)->axis;
}
