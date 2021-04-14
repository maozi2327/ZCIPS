#include "stdafx.h"
#include "axiscontrolwidget.h"
#include "controllerinterface.h"
#include "../Public/util/functions.h"

AxisControlwidget::AxisControlwidget(ControllerInterface* _controller, const std::map<Axis, AxisData>& _axisDataMap, QWidget *parent)
	: QWidget(parent), d_controller(_controller)
{
	for (auto& itr : _axisDataMap)
	{
		auto controlsCombo = new AxisControl(itr.first, itr.second.axisCaption, itr.second.axisName, this);
		//controlsCombo->radioButton->setMinimumSize(QSize(13, 0));
		//controlsCombo->radioButton->setMaximumSize(QSize(13, 16777215));
		//controlsCombo->radioButton->setFixedHeight(20);
		//controlsCombo->axisNameLabel->setMinimumSize(QSize(60, 0));
		//controlsCombo->axisNameLabel->setMaximumSize(QSize(60, 16777215));
		controlsCombo->axisPosLabel->setMinimumSize(QSize(60, 20));
		//controlsCombo->axisPosLabel->setMaximumSize(QSize(60, 16777215));
		//controlsCombo->axisPosLabel->setFixedHeight(20);
		controlsCombo->axisPosLabel->setFrameShape(QFrame::Box);
		d_axisControls.push_back(controlsCombo);
	}
	auto controlsCombo = new AxisControl(Axis::layer1layer2, QString::fromLocal8Bit("分层同步"), QString::fromLocal8Bit("layer1layer2"), this);
	d_axisControls.push_back(controlsCombo);
	controlsCombo = new AxisControl(Axis::rayTranslationDetTranslation, QString::fromLocal8Bit("平移同步"), QString::fromLocal8Bit("rayTranslationDetTranslation"), this);
	d_axisControls.push_back(controlsCombo);

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
		//d_axisGridLayout->addWidget(d_axisControls[i]->axisNameLabel, i, pos++, 1, 1);
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
	d_posLineEdit->setMinimumSize(QSize(60, 15));
	//d_posLineEdit->setMaximumSize(QSize(60, 15));

	d_posAreaGridLayout->addWidget(d_posLineEdit, 0, 0, 1, 2);

	d_negativePosButton = new QPushButton(this);
	d_negativePosButton->setObjectName(QStringLiteral("negativePosButton"));
	d_negativePosButton->setText(QString::fromLocal8Bit("反"));
	d_negativePosButton->setMinimumSize(QSize(25, 25));
	//d_negativePosButton->setMaximumSize(QSize(25, 25));

	d_posAreaGridLayout->addWidget(d_negativePosButton, 1, 0, 1, 1);

	d_positivePosButton = new QPushButton(this);
	d_positivePosButton->setObjectName(QStringLiteral("positivePosButton"));
	d_positivePosButton->setText(QString::fromLocal8Bit("正"));
	d_positivePosButton->setMinimumSize(QSize(25, 25));
	//d_positivePosButton->setMaximumSize(QSize(25, 25));

	d_posAreaGridLayout->addWidget(d_positivePosButton, 1, 1, 1, 1);

	d_absPosButton = new QPushButton(this);
	d_absPosButton->setObjectName(QStringLiteral("absPosButton"));
	d_absPosButton->setText(QString::fromLocal8Bit("绝对定位"));
	d_absPosButton->setMinimumSize(QSize(30, 30));
	//d_absPosButton->setMaximumSize(QSize(70, 30));

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
	//d_stopButton->setMaximumSize(QSize(100, 50));

	d_posAreaGridLayout->addWidget(d_stopButton, 3, 0, 1, 2);

	d_horizontalLayout->addLayout(d_posAreaGridLayout);

	d_axisRadioButtonGroup = new QButtonGroup(this);
	d_axisRadioButtonGroup->setExclusive(true);

	d_axisControls[0]->radioButton->setChecked(true);
	d_axisSelected = d_axisControls[0]->axis;

	auto axisControlsHeight = _axisDataMap.size() * (20 + 6) +
		d_axisGridLayout->contentsMargins().top() + d_axisGridLayout->contentsMargins().bottom();

	auto buttonsHeight = d_posLineEdit->height() + d_negativePosButton->height() + d_absPosButton->height() + d_stopButton->height() +
		d_posAreaGridLayout->contentsMargins().top() + d_posAreaGridLayout->contentsMargins().bottom() + 10 * 3;

	auto height = axisControlsHeight > buttonsHeight ? axisControlsHeight : buttonsHeight;
	setFixedHeight(height + d_horizontalLayout->contentsMargins().top() + d_horizontalLayout->contentsMargins().bottom());
	//	d_axisGridLayout->contentsMargins().top() + d_axisGridLayout->contentsMargins().bottom() +
	//	d_horizontalLayout->contentsMargins().top() + d_horizontalLayout->contentsMargins().bottom());
	ui.setupUi(this);

	for (auto& itr : d_axisControls)
		d_axisRadioButtonGroup->addButton(itr->radioButton);

	connect(d_axisRadioButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
		this, SLOT(onRadionButtonClicked(QAbstractButton*)));

	setLineEditValidaterFloatChar(d_posLineEdit);
}

AxisControlwidget::~AxisControlwidget()
{
}

void AxisControlwidget::updateControlSts(bool _enable)
{
	d_negativePosButton->setEnabled(_enable);
	d_positivePosButton->setEnabled(_enable);
	d_absPosButton->setEnabled(_enable);
}

void AxisControlwidget::on_negativePosButton_clicked()
{
	emit negativePosButtonSignal();
}
void AxisControlwidget::updateStatus(const std::map<Axis, float>& _axisPos)
{
	for (auto& itr : d_axisControls)
	{
		auto axisPosEnumItr = AxisNamePosMap.find(itr->axis);

		if (axisPosEnumItr != AxisNamePosMap.end())
		{
			auto axisItr = _axisPos.find(itr->axis);

			if(axisItr != _axisPos.end())
				itr->axisPosLabel->
					setText(QString("%1").arg(axisItr->second, 0, 'f', 2));
		}
	}
}
void AxisControlwidget::updateAxisStatus()
{
}


void AxisControlwidget::on_positivePosButton_clicked()
{
	emit positivePosButtonSignal();
}
void AxisControlwidget::on_stopButton_clicked()
{
	emit stopButtonSignal();
}
void AxisControlwidget::on_absPosButton_clicked()
{
	emit absPosButtonSignal();
}
void AxisControlwidget::onRadionButtonClicked(QAbstractButton * _button)
{
	auto itr = std::find_if(d_axisControls.begin(), d_axisControls.end(),
		[=](AxisControl* _ptr) { return _ptr->radioButton == _button; });
	
	if (itr != d_axisControls.end())
		d_axisSelected = (*itr)->axis;
}
